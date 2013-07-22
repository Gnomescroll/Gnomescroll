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

char* extract_errors(const char* response)
{
    const int n_tokens = 64;
    jsmntok_t token[n_tokens];
    for (int i=0; i<n_tokens; i++)
        tokens[i].size = -1;
    jsmn_init(&parser);
    jsmnerr_t r = jsmn_parse(&parser, response, tokens, n_tokens);
    IF_ASSERT(r != JSMN_SUCCESS)
        return NULL;

    const char name[] = "errors";
    bool name_found = false;
    char* errstr = (char*)calloc(1, sizeof(*errstr));
    for (int i=0; i<n_tokens && tokens[i].size >= 0; i++)
    {
        int len = tokens[i].end - tokens[i].start;
        IF_ASSERT(tokens[i].start < 0) break;
        IF_ASSERT(len < 0) break;
        const char* word = &response[tokens[i].start];
        if (name_found)
        {
            IF_ASSERT(tokens[i].type != JSMN_OBJECT)
                break;
            char* csrf = (char*)malloc(len + 1);
            // TODO
        }
    }
    return NULL;
}

char* extract_csrf_token(const char* response)
{
    const int n_tokens = 8;
    jsmntok_t tokens[n_tokens];
    for (int i=0; i<n_tokens; i++)
        tokens[i].size = -1;
    jsmn_init(&parser);
    jsmnerr_t r = jsmn_parse(&parser, response, tokens, n_tokens);
    IF_ASSERT(r != JSMN_SUCCESS)
        return NULL;

    const char name[] = "csrf_token";
    bool name_found = false;
    for (int i=0; i<n_tokens && tokens[i].size >= 0; i++)
    {
        int len = tokens[i].end - tokens[i].start;
        IF_ASSERT(tokens[i].start < 0) break;
        IF_ASSERT(len < 0) break;
        const char* word = &response[tokens[i].start];
        if (name_found)
        {
            IF_ASSERT(tokens[i].type != JSMN_STRING)
                break;
            char* csrf = (char*)malloc(len + 1);
            strncpy(csrf, word, len);
            csrf[len] = '\0';
            return csrf;
        }
        if (tokens[i].type != JSMN_STRING)
            continue;
        name_found = (strncmp(word, name, sizeof(name) - 1) == 0);
    }
    return NULL;
}

char* extract_auth_token(const char* response)
{
    const int n_tokens = 32;
    jsmntok_t tokens[n_tokens];
    for (int i=0; i<n_tokens; i++)
        tokens[i].size = -1;
    jsmn_init(&parser);
    jsmnerr_t r = jsmn_parse(&parser, response, tokens, n_tokens);
    IF_ASSERT(r != JSMN_SUCCESS)
        return NULL;
    const char* name = AUTH_TOKEN_NAME;
    bool found = false;
    for (int i=0; i<n_tokens && tokens[i].size >= 0; i++)
    {
        int len = tokens[i].end - tokens[i].start;
        IF_ASSERT(tokens[i].start < 0) break;
        IF_ASSERT(len < 0) break;
        const char* word = &response[tokens[i].start];
        if (found)
        {
            IF_ASSERT(tokens[i].type != JSMN_STRING)
                break;
            char* token = (char*)malloc(len + 1);
            strncpy(token, word, len);
            token[len] = '\0';
            return token;
        }
        if (tokens[i].type != JSMN_STRING)
            continue;
        found = (strncmp(word, name, sizeof(name) - 1) == 0);
    }
    return NULL;
}

jsmnerr_t jsmn_parse(jsmn_parser *parser, const char *js,
        jsmntok_t *tokens, unsigned int num_tokens);


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

    char* token = extract_csrf_token(response.memory);
    IF_ASSERT(token == NULL)
    {
        printf("Could not parse csrf token from ");
        response.print();
        success = false;
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
        char* auth_token = extract_auth_token(response.memory);
        if (auth_token == NULL)
        {
            #if !PRODUCTION
            printf("Could not parse auth token from ");
            response.print();
            #endif
            success = false;
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
                Hud::set_awesomium_message("Authentication server failure. Try again soon.");
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

bool login(const char* username, const char* password)
{
    if (!check_version()) return false;
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
    CurlData response;
    _make_request(GNOMESCROLL_URL GNOMESCROLL_VERSION_PATH, &response);
    IF_ASSERT(strcmp(response.memory, GS_STR(GS_VERSION)) != 0)
    {
        printf("WARNING: Version mismatch. Web server version: %s. "
               "Local version: %d\n", response.memory, GS_VERSION);
        Hud::set_awesomium_message("Your client is out of date. Get the new version at " GNOMESCROLL_DOMAIN);
        return false;
    }
    return true;
}

}   // Auth
