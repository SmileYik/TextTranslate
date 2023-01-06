// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#ifndef HTTP_ACTION_H
#define HTTP_ACTION_H

#include "based_type.h"

namespace HttpAction {

#define HTTP "http"
#define HTTPS "https"
#define HTTP_PORT "80"
#define HTTPS_PORT "443"

#define PORT_BUFFER_LIMIT 50
#define HOST_BUFFER_LIMIT 1024

#define HTTP_VERSION "HTTP/1.0"
#define GET "GET"
#define POST "POST"

/**
 * Url类, 用来确定连接信息.
 */
class Url {
public:
    String scheme = "http";     // 协议
    String authority = "";      // 主机
    String port = "";           // 端口
    String path = "";           // 路径

    Url() = default;
    Url(const String& urlConst);

    /**
     * 获取URL
     */
    const String getUrl();

    /**
     * 获取URL
     */
    void getUrl(String* url);

};

/**
 * HTTP GET 方法
 * @param url 字符串链接
 * @param params 查询参数
 * @param respBody GET方法返回的内容. 为NULL时不会返回此内容.
 * @param respMessage GET请求时返回的消息. 为NULL时不会返回此内容.
 * @param respHeaders GET请求时服务器返回的响应标头. 为NULL时不会返回此内容.
 * @param headers 请求标头.
 * @param cookies 请求时用的 cookie.
 * @return HTTP 请求状态码.
 */
int get(String url, const StringMap& params = {}, String* respBody = NULL,
        String* respMessage = NULL, StringMultiMap* respHeaders = nullptr,
        const StringMultiMap& headers = {}, const StringMap& cookies = {});


} // namespace HttpAction

#endif // HTTP_ACTION_H
