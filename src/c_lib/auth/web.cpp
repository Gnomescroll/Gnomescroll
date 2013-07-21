#include "web.hpp"

#include <stdio.h>
#include <curl/curl.h>

namespace Auth
{

/* Example cookie handling copied from:
 *  http://curl.haxx.se/libcurl/c/cookie_interface.html
 * Example SSL request copied from:
 *  http://curl.haxx.se/libcurl/c/https.html
 */

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

void download_homepage()
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
        curl_easy_setopt(curl, CURLOPT_CAINFO, MEDIA_PATH "./certs/gnomescroll.pem");

        curl_easy_setopt(curl, CURLOPT_URL, GNOMESCROLL_URL);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */

        #if !PRODUCTION
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        #endif

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));

        print_cookies(curl);

        printf("Erasing curl's knowledge of cookies!\n");
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");

        print_cookies(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

}   // Auth
