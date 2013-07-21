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
        this->memory = (char*)malloc(1);
        this->size = 0;
    }

    ~CurlData()
    {
        free(this->memory);
    }
};

static jsmn_parser parser;

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

void log_curl_error(CURLcode res)
{
    IF_ASSERT(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
}

void login(const char* username, const char* password)
{
    CURL* curl = curl_easy_init();
    IF_ASSERT(curl == NULL) return;

    class CurlData response;
    struct curl_slist* chunk = NULL;
    setup_auth_request(curl, &response, chunk,
                       GNOMESCROLL_URL GNOMESCROLL_LOGIN_PATH);

    // make the request
    CURLcode res = curl_easy_perform(curl);
    log_curl_error(res);

    //print_cookies(curl);
    //printf("Erasing curl's knowledge of cookies!\n");
    //curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
    //print_cookies(curl);

    response.print();
    char* token = extract_csrf_token(response.memory);
    IF_ASSERT(token == NULL)
    {
        printf("Could not parse csrf token from ");
        response.print();
    }
    else
    {
        response.reset();
        printf("Token: %s\n", token);
        // now do login
        const char fmt[] = "username_or_email=%s&password=%s&csrf_token=%s&version=" GS_STR(GS_VERSION);
        size_t len = strlen(username) + strlen(password) + strlen(token) + sizeof(fmt);
        char* post = (char*)malloc(len);
        sprintf(post, fmt, username, password, token);
        printf("Sending post vars: %s\n", post);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
        res = curl_easy_perform(curl);
        log_curl_error(res);
        free(post);
        response.print();
        char* auth_token = extract_auth_token(response.memory);
        IF_ASSERT(auth_token == NULL)
        {
            printf("Could not parse auth token from ");
            response.print();
        }
        else
        {
            printf("Auth token: %s\n", auth_token);
            AuthError error = update_token(auth_token);
            IF_ASSERT(error != AUTH_ERROR_NONE)
            {
                printf("Auth error code: %d\n", error);
                Hud::set_awesomium_message("Authentication server failure. Try again soon.");
                token_failure = true;
            }
        }
        free(auth_token);
    }

    free(token);
    curl_easy_cleanup(curl);
}

// username_or_email
// password
// csrf_token

#undef INIT_RESPONSE

}   // Auth
