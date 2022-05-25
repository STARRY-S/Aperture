#include "ap_network.h"
#include "ap_utils.h"

#ifndef AP_PLATFORM_ANDROID
#include <curl/curl.h>
#endif

#include <string.h>

#define MAX_POST_LENGTH 1024
#define MAX_RESPONSE_LENGTH 1024

size_t write_func_cb(char *data, size_t size, size_t count, void *arg)
{
        size_t length = size * count;
        char *dst = (char *)arg;
        char *src = malloc(length + 1);
        memcpy(src, data, length);
        src[length] = '\0';
        strncat(dst, src, MAX_RESPONSE_LENGTH - strlen(dst) - 1);
        free(src);
        return length;
}

int ap_network_send(char *url, char *data, char *result, int length)
{
#ifdef AP_PLATFORM_ANDROID
        return 0;
#else
        if (!data || !url || !result) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (strlen(data) > MAX_POST_LENGTH) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        CURL *curl = curl_easy_init();
        if (curl) {
                char post[MAX_POST_LENGTH] = {0};
                char response[MAX_RESPONSE_LENGTH] = {0};
                strcpy(post, data);
                long http_code = 0;

                #ifdef _WIN32
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
                #endif
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func_cb);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);

                CURLcode code = curl_easy_perform(curl);
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                curl_easy_cleanup(curl);

                if (code == CURLE_OK && http_code == 200) {
                        strncpy(result, response, length);
                        return 1;
                }
        }
        return 0;
#endif
}