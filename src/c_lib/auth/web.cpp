#include "web.hpp"

#include <stdio.h>
#include <curl/curl.h>
#include <common/json/jsmn.h>


namespace Auth
{

/* Example cookie handling copied from:
 *  http://curl.haxx.se/libcurl/c/cookie_interface.html
 * Example SSL request copied from:
 *  http://curl.haxx.se/libcurl/c/https.html
 */

static bool _version_good = false;

class CurlData
{
    public:
        char* memory;
        size_t size;

    void print()
    {
        printf("Response: %s\n", this->memory);
    }

    bool grow_to(size_t new_size)
    {
        char* tmp = (char*)realloc(this->memory, this->size + new_size);
        IF_ASSERT(tmp == NULL)
            return false;
        this->memory = tmp;
        return true;
    }

    void load(const char* data, size_t data_size)
    {
        strncpy(&(this->memory[this->size]), data, data_size);
        this->size += data_size;
        this->memory[this->size] = '\0';
    }

    void reset()
    {
        this->size = 0;
    }

    CurlData()
    {
        this->memory = (char*)calloc(1, sizeof(*this->memory));
        this->size = 0;
    }

    ~CurlData()
    {
        free(this->memory);
    }
};

static jsmn_parser parser;

char* _get_json_value(const char* json, const char* value_name,
                      jsmntype_t value_type, jsmntok_t* tokens, size_t n_tokens)
{
    for (size_t i=0; i<n_tokens; i++)
        tokens[i].size = -1;
    jsmn_init(&parser);
    jsmnerr_t r = jsmn_parse(&parser, json, tokens, n_tokens);
    IF_ASSERT(r != JSMN_SUCCESS)
        return NULL;

    bool name_found = false;
    for (size_t i=0; i<n_tokens && tokens[i].size >= 0; i++)
    {
        int len = tokens[i].end - tokens[i].start;
        IF_ASSERT(tokens[i].start < 0) break;
        IF_ASSERT(len < 0) break;
        const char* word = &json[tokens[i].start];
        if (name_found)
        {
            IF_ASSERT(tokens[i].type != value_type)
                break;
            char* value = (char*)malloc(len + 1);
            strncpy(value, word, len);
            value[len] = '\0';
            return value;
        }
        if (tokens[i].type != JSMN_STRING)
            continue;
        name_found = (strncmp(word, value_name, len) == 0);
    }
    return NULL;
}

bool response_was_successful(const char* response)
{   // look for "success": true in the json response
    const size_t n_tokens = 32;
    jsmntok_t tokens[n_tokens];
    char* success = _get_json_value(response, "success", JSMN_PRIMITIVE, tokens, n_tokens);
    if (success == NULL)    // no "success" found; N/A
        return true;
    char c = success[0];
    free(success);
    return (c == 't');
}

char* extract_errors(const char* response)
{
    const size_t n_tokens = 64;
    jsmntok_t tokens[n_tokens];
    char* errors_obj = _get_json_value(response, "errors", JSMN_OBJECT, tokens, n_tokens);
    if (errors_obj == NULL)
        return NULL;

    for (size_t i=0; i<n_tokens; i++)
        tokens[i].size = -1;
    jsmn_init(&parser);
    jsmnerr_t r = jsmn_parse(&parser, errors_obj, tokens, n_tokens);
    IF_ASSERT(r != JSMN_SUCCESS)
        return NULL;

    size_t errors_len = 128;
    size_t errors_index = 0;
    char* errors = (char*)calloc(errors_len, sizeof(*errors));
    char* label = (char*)calloc(1, sizeof(*label));
    bool success = true;
    bool expecting_label = true;
    int errors_parent = -1;
    for (size_t i=0; i<n_tokens && tokens[i].size >= 0; i++)
    {
        int len = tokens[i].end - tokens[i].start;
        IF_ASSERT(tokens[i].start < 0 || len < 0)
        {
            success = false;
            break;
        }
        const char* word = &errors_obj[tokens[i].start];

        if (tokens[i].parent != errors_parent)
            expecting_label = true;

        if (tokens[i].parent == errors_parent && tokens[i].type == JSMN_STRING)
        {
            char* error = (char*)malloc(len + 1);
            strncpy(error, word, len);
            error[len] = '\0';
            const char fmt[] = "%s: %s\n";
            size_t needed = strlen(label) + strlen(error) + sizeof(fmt);
            size_t space = errors_len - errors_index;
            if (needed > space)
            {
                size_t new_len = errors_len;
                while (new_len < needed)
                    new_len *= 2;
                char* tmp = (char*)realloc(errors, new_len);
                IF_ASSERT(tmp == NULL)
                {
                    free(error);
                    success = false;
                    break;
                }
                errors = tmp;
                errors_len = new_len;
            }
            size_t wrote = sprintf(&errors[errors_index], fmt, label, error);
            errors_index += wrote;
            GS_ASSERT(errors_index < errors_len);
            free(error);
        }

        // pull out label
        if (tokens[i].type == JSMN_STRING && expecting_label)
        {
            expecting_label = false;
            char* tmp = (char*)realloc(label, len + 1);
            IF_ASSERT(tmp == NULL)
            {
                success = false;
                break;
            }
            label = tmp;
            strncpy(label, word, len);
            label[len] = '\0';
        }

        if (tokens[i].type == JSMN_ARRAY)
            errors_parent = i;
    }

    free(label);
    free(errors_obj);
    if (!success)
    {
        free(errors);
        return NULL;
    }
    return errors;
}

char* extract_csrf_token(const char* response, bool& error)
{
    error = false;
    const size_t n_tokens = 8;
    jsmntok_t tokens[n_tokens];
    if (!response_was_successful(response))
    {
        error = true;
        return extract_errors(response);
    }
    return _get_json_value(response, "csrf_token", JSMN_STRING, tokens, n_tokens);
}

char* extract_auth_token(const char* response, bool& error)
{
    error = false;
    const size_t n_tokens = 32;
    jsmntok_t tokens[n_tokens];
    if (!response_was_successful(response))
    {
        error = true;
        return extract_errors(response);
    }
    return _get_json_value(response, AUTH_TOKEN_NAME, JSMN_STRING, tokens, n_tokens);
}

static size_t _write_data_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    class CurlData* mem = reinterpret_cast<CurlData*>(userp);
    if (!mem->grow_to(realsize + 1))
        return 0;
    mem->load((const char*)contents, realsize);
    return realsize;
}

static void print_cookies(CURL *curl)
{
  CURLcode res;
  struct curl_slist *cookies;
  struct curl_slist *nc;
  int i;

  printf("Cookies, curl knows:\n");
  res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
  if (res != CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
    exit(1);
  }
  nc = cookies, i = 1;
  while (nc) {
    printf("[%d]: %s\n", i, nc->data);
    nc = nc->next;
    i++;
  }
  if (i == 1) {
    printf("(none)\n");
  }
  curl_slist_free_all(cookies);
}

void setup_ssl(CURL* curl)
{
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_CAINFO, MEDIA_PATH "./certs/gnomescroll.pem");
}

void add_ajax_header(CURL* curl, struct curl_slist*& chunk)
{
    chunk = curl_slist_append(chunk, "X-Requested-With: XMLHttpRequest");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
}

void set_verbosity(CURL* curl)
{
    #if !PRODUCTION
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif
}

void setup_auth_request(CURL* curl, class CurlData* response,
                        struct curl_slist*& chunk, const char* url)
{
    set_verbosity(curl);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &_write_data_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);
    add_ajax_header(curl, chunk);
    setup_ssl(curl);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 4L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "gnomescroll/" GS_STR(GS_VERSION));
    curl_easy_setopt(curl, CURLOPT_URL, url);
}

void init_curl()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void teardown_curl()
{
    curl_global_cleanup();
}

bool log_curl_error(CURLcode res)
{   // returns true if error
    IF_ASSERT(res != CURLE_OK)
    {
        printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return true;
    }
    return false;
}

bool _make_auth_post_request(const char* url, const char* post_data)
{
    #if !PRODUCTION
    printf("Making auth post request to: %s\n", url);
    #endif

    CURL* curl = curl_easy_init();
    IF_ASSERT(curl == NULL) return false;

    bool success = true;
    class CurlData response;
    struct curl_slist* chunk = NULL;
    setup_auth_request(curl, &response, chunk, url);

    // get the csrf token
    CURLcode res = curl_easy_perform(curl);
    if (log_curl_error(res))
        success = false;

    #if !PRODUCTION
    response.print();
    #endif

    bool had_error = false;
    char* token = extract_csrf_token(response.memory, had_error);
    IF_ASSERT(token == NULL)
    {
        printf("Could not parse csrf token from ");
        response.print();
        success = false;
    }
    else if (had_error)
    {
        success = false;
        printf("Response errors:\n%s\n", auth_token);
        Hud::set_login_message(token);
    }
    else
    {   // send the actual request, with the csrf token
        const char fmt[] = "csrf_token=%s&%s";
        size_t len = sizeof(fmt) + strlen(post_data) + strlen(token);
        char* post = (char*)malloc(len);
        size_t wrote = snprintf(post, len, fmt, token, post_data);
        GS_ASSERT(wrote < len);
        #if !PRODUCTION
        printf("CSRF Token: %s\n", token);
        printf("Sending post vars: %s\n", post);
        #endif
        response.reset();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
        res = curl_easy_perform(curl);
        if (log_curl_error(res))
            success = false;
        response.print();
        free(post);
        char* auth_token = extract_auth_token(response.memory, had_error);
        if (auth_token == NULL)
        {
            #if !PRODUCTION
            printf("Could not parse auth token from ");
            response.print();
            #endif
            success = false;
        }
        else if (had_error)
        {
            success = false;
            printf("Response errors:\n%s\n", auth_token);
            Hud::set_login_message(auth_token);
        }
        else
        {
            #if !PRODUCTION
            printf("Auth token: %s\n", auth_token);
            #endif
            AuthError error = update_token(auth_token);
            IF_ASSERT(error != AUTH_ERROR_NONE)
            {
                printf("Auth error code: %d\n", error);
                Hud::set_login_message("Authentication server failure. Try again soon.");
                token_failure = true;
                success = false;
            }
        }
        free(auth_token);
    }

    free(token);
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");  // clear cookies
    curl_easy_cleanup(curl);

    return success;
}

bool _make_request(const char* url, CurlData* response)
{
    #if !PRODUCTION
    printf("Making get request to: %s\n", url);
    #endif

    CURL* curl = curl_easy_init();
    IF_ASSERT(curl == NULL) return false;

    struct curl_slist* chunk = NULL;
    setup_auth_request(curl, response, chunk, url);
    CURLcode res = curl_easy_perform(curl);
    log_curl_error(res);

    #if !PRODUCTION
    response->print();
    #endif

    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");  // clear cookies
    curl_easy_cleanup(curl);
    return (res == CURLE_OK);
}

void _update_hud_message(const char* msg)
{   // we have to draw() here because we are about to block on the web request
    Hud::set_login_message(msg);
    ClientState::update_camera();
    world_projection();
    hud_projection();
    Hud::draw_login_message();
    Hud::draw();
    CHECK_GL_ERROR();
    _swap_buffers();
}

bool login(const char* username, const char* password)
{
    if (!check_version()) return false;
    _update_hud_message("Logging in...");
    const char fmt[] = "username_or_email=%s&password=%s&version=" GS_STR(GS_VERSION);
    size_t len = strlen(username) + strlen(password) + sizeof(fmt);
    char* post = (char*)malloc(len);
    size_t wrote = sprintf(post, fmt, username, password);
    GS_ASSERT(wrote < len);
    bool good = _make_auth_post_request(GNOMESCROLL_URL GNOMESCROLL_LOGIN_PATH, post);
    free(post);
    return good;
}

bool create_account(const char* username, const char* email, const char* password)
{
    if (!check_version()) return false;
    _update_hud_message("Creating account...");
    const char fmt[] = "username=%s&email_address=%s&passwords_password=%s&passwords_confirm=%s&subscribe=true&version=" GS_STR(GS_VERSION);
    size_t len = strlen(username) + strlen(password) * 2 + strlen(email) + sizeof(fmt);
    char* post = (char*)malloc(len);
    size_t wrote = snprintf(post, len, fmt, username, email, password, password);
    GS_ASSERT(wrote < len);
    bool good = _make_auth_post_request(GNOMESCROLL_URL GNOMESCROLL_CREATE_PATH, post);
    free(post);
    return good;
}

bool check_version()
{
    if (_version_good) return true;
    _update_hud_message("Checking version...");
    CurlData response;
    _make_request(GNOMESCROLL_URL GNOMESCROLL_VERSION_PATH, &response);
    IF_ASSERT(strcmp(response.memory, GS_STR(GS_VERSION)) != 0)
    {
        printf("WARNING: Version mismatch. Web server version: %s. "
               "Local version: %d\n", response.memory, GS_VERSION);
        Hud::set_login_message("Your client is out of date. Get the new version at " GNOMESCROLL_DOMAIN);
        return false;
    }
    _version_good = true;
    return true;
}

}   // Auth
