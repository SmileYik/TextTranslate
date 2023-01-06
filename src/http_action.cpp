// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#include "http_action.h"
#include <cstring>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "rio.h"

namespace HttpAction {

u_char toHex(u_char x) {
    return  x > 9 ? x + 55 : x + 48;
}

u_char fromHex(u_char x) {
    u_char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}

/**
 * url编码
 */
String urlEncode(const String& str) {
    String strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((u_char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += toHex((u_char)str[i] >> 4);
            strTemp += toHex((u_char)str[i] % 16);
        }
    }
    return strTemp;
}

/**
 * url解码
 */
String urlDecode(const String& str) {
    String strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            u_char high = fromHex((u_char)str[++i]);
            u_char low = fromHex((u_char)str[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}

/**
 * 将字符串生成一个其小写字母版本.
 */
const String strToLower(const String& str) {
    String out = "";
    for (const auto& c : str) {
        if (c >= 'A' && c <= 'Z') {
            out += (char) (c + 48);
        } else {
            out += c;
        }
    }
    return out;
}

/**
 * 打开一个Socket连接
 *
 * @param server 服务器地址
 * @param port   端口号
 * @return 返回-1则表示获取服务器地址信息失败,
 *         返回-2则表示连接到服务器失败,
 *         大于等于0则表示连接成功.
 */
int openClientfd(const char* server, const char* port) {
    addrinfo hints;
    addrinfo* hostList;
    addrinfo* hostPtr;
    int errorCode;
    int clientfd = -1;

    memset(&hints, 0, sizeof(hints));
    // hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;

    // 搜寻ip地址.
    errorCode = getaddrinfo(server, port, &hints, &hostList);
    if (errorCode != 0) {
        if (ENABLE_ERROR_LOG) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errorCode));
        }
        return -1;
    }

    // 根据搜寻到的ip, 依次尝试连接.
    for (hostPtr = hostList; hostPtr; hostPtr = hostPtr->ai_next) {
        // 显示当先连接的ip地址.
        if (ENABLE_DEBUG_LOG) {
            char hostAddr[HOST_BUFFER_LIMIT];
            char hostPort[PORT_BUFFER_LIMIT];
            errorCode = getnameinfo(hostPtr->ai_addr, hostPtr->ai_addrlen,
                                    hostAddr, HOST_BUFFER_LIMIT,
                                    hostPort, PORT_BUFFER_LIMIT,
                                    NI_NUMERICSERV);

            if (errorCode == 0) {
                printf("try to connect to '%s:%s'\n", hostAddr, hostPort);
            } else {
                printf("try to connect to 'Unknown Address'\n");
            }
        }

        clientfd = socket(hostPtr->ai_family,
                          hostPtr->ai_socktype, hostPtr->ai_protocol);
        if (clientfd < 0) {
            continue;
        }

        if (connect(clientfd, hostPtr->ai_addr, hostPtr->ai_addrlen) != -1) {
            break;
        }
        close(clientfd);
    }

    freeaddrinfo(hostList);

    // 如果没有连接成功那么应该是遍历完了整个链表.
    if (!hostPtr) {
        if (ENABLE_ERROR_LOG) {
            fprintf(stderr, "Can not connect to '%s:%s'.\n", server, port);
        }
        return -2;
    }

    return clientfd;
}

/**
 * 发送一条HTTP请求
 * @param actionType 请求类型.
 * @param url Url.
 * @param requestHeaders 请求标头.
 * @param requestBody 请求体.
 * @param responseHeaders 响应标头.
 * @param responeseMessage 响应信息.
 * @param responeseBody 响应体.
 * @return http状态代码. 请求响应时发生错误, 如连接错误等返回-1.
 */
int httpAction(const String& actionType, const Url& url,
               const StringMultiMap& requestHeaders, const String* requestBody,
               StringMultiMap* responseHeaders, String* responeseMessage,
               String* responeseBody) {

    if (url.port.empty()) {
        return -1;
    }

    StringMap additionHeaders;

    if (requestHeaders.find("Host") == requestHeaders.end()) {
        if (!(url.scheme == HTTP && url.port == HTTP_PORT) &&
            !(url.scheme == HTTPS && url.port == HTTPS_PORT)) {
            additionHeaders["Host"] = url.authority + ":" + url.port;
        } else {
            additionHeaders["Host"] = url.authority;
        }
    }
    if (requestHeaders.find("Accept") == requestHeaders.end()) {
        additionHeaders["Accept"] = "*/*";
    }
    if (requestHeaders.find("User-Agent") == requestHeaders.end()) {
        additionHeaders["User-Agent"] = "http_action/1.0.0";
    }

    int clientfd;
    String content = "";
    // request line
    content += actionType + " " + url.path + " " + HTTP_VERSION + "\r\n";
    // header line;
    for (const auto& entry : requestHeaders) {
        content += entry.first + ": " + entry.second + "\r\n";
    }
    for (const auto& entry : additionHeaders) {
        content += entry.first + ": " + entry.second + "\r\n";
    }
    content += "\r\n";
    // request body
    // TODO 检查请求方法来附带请求体
    if (requestBody && requestBody != nullptr) {
        content += *requestBody + "\r\n";
    }

    // open connection
    clientfd = openClientfd(url.authority.c_str(), url.port.c_str());
    if (clientfd < 0) {
        return -1;
    }

    const char* contentStr = content.c_str();
    if (ENABLE_DEBUG_LOG) {
        printf("Send content to server: \n%s\n--------------\n", contentStr);
    }
    if (Rio::rio_writen(clientfd, (void *) contentStr, strlen(contentStr)) == -1) {
        close(clientfd);
        return -1;
    }

    String ret = "";
    char buffer[RIO_BUFFER_SIZE];
    ssize_t n;
    while (true) {
        n = Rio::rio_readn(clientfd, (void *) buffer, RIO_BUFFER_SIZE);
        if (n > 0) {
            ret.append(buffer, 0, n);
        } else if (n == -1) {
            if (ENABLE_ERROR_LOG) {
                fprintf(stderr, "read responese from server error!\n");
            }
            break;
        } else {
            break;
        }
    }
    close(clientfd);
    int code = -1;

    if (ret.empty()) {
        return code;
    }

    try {
        int idx;
        while ((idx = ret.find_first_of('\r')) != String::npos) {
            ret.replace(idx, 1, "");
        }
        // first line
        // HTTP/1.1 200 OK
        idx = ret.find_first_of('\n');
        String tmp = ret.substr(0, idx);
        idx = tmp.find_first_of(' ');
        int i = tmp.find_first_of(' ', idx + 1);
        sscanf(tmp.substr(idx + 1, i - idx - 1).c_str(), "%d", &code);
        if (responeseMessage && responeseMessage != nullptr) {
            *responeseMessage = tmp.substr(i + 1);
        }
        // header line;
        if (responseHeaders && responseHeaders != nullptr) {
            idx = ret.find_first_of('\n');
            i = ret.find("\n\n");
            tmp = ret.substr(idx + 1, i - idx);
            String line;
            while (!tmp.empty()) {
                idx = tmp.find_first_of('\n');
                if (idx == String::npos) {
                    break;
                }
                line = tmp.substr(0, idx);
                tmp = tmp.substr(idx + 1);
                idx = line.find_first_of(":");
                if (idx == String::npos) {
                    continue;
                }
                responseHeaders->insert({
                    line.substr(0, idx),
                    line.substr(idx + 2, line.length() - idx - 2)
                });
            }
        }

        // body
        if (responeseBody && responeseBody != nullptr) {
            i = ret.find("\n\n") + 2;
            *responeseBody = ret.substr(i);
        }
    } catch (error_t e) {

    }

    return code;
}




Url::Url(const String& urlConst) {
    // http://abc:456/index?abc=456
    int i;
    // find scheme
    int idx = urlConst.find_first_of(':');
    if (idx != String::npos) {
        scheme = urlConst.substr(0, idx);
        idx += 3;
    } else {
        idx = 0;
    }

    // find host:port
    i = urlConst.find_first_of('/', idx);
    if (i == String::npos) {
        i = urlConst.length();
    }
    authority = urlConst.substr(idx, i - idx);
    idx = authority.find_first_of(':');
    if (idx >= 0) {
        port = authority.substr(idx + 1);
        authority = authority.substr(0, idx);
    } else {
        const String str = strToLower(scheme);
        if (str == HTTP) {
            port = HTTP_PORT;
        } else if (str == HTTPS) {
            port = HTTPS_PORT;
        }
    }
    if (i == urlConst.length()) {
        return;
    }

    path = urlConst.substr(i);
}

void Url::getUrl(String* url) {
    *url = getUrl();
}

const String Url::getUrl() {
    String url = "";
    if (!scheme.empty()) {
        url += scheme + "://";
    }
    if (!authority.empty()) {
        url += authority;
    }
    if (!port.empty()) {
        const String schemeTmp = strToLower(scheme);
        if (!(schemeTmp == HTTP && port == HTTP_PORT) &&
            !(schemeTmp == HTTPS && port == HTTPS_PORT)) {
            url += ":" + port;
        }
    }
    if (!path.empty()) {
        url += path;
    }
    return url;
}

int get(String url, const StringMap& params, String* respBody,
        String* respMessage, StringMultiMap* respHeaders,
        const StringMultiMap& headers, const StringMap& cookies) {
    String paramsStr = "";
    for (const auto& param : params) {
        paramsStr += "&" + param.first + "=" + urlEncode(param.second);
    }
    if (!paramsStr.empty()) {
        paramsStr[0] = '?';
    }
    url += paramsStr;
    Url u(url);
    if (u.path.empty()) {
        u.path = "/";
    }

    paramsStr = "";

    for (const auto& param : cookies) {
        paramsStr += param.first + "=" + urlEncode(param.second) + "; ";
    }

    StringMultiMap newHeaders(headers);
    newHeaders.insert({"cookie", paramsStr});

    return httpAction(GET, u, newHeaders, nullptr, respHeaders, respMessage, respBody);
}

} // namespace HttpAction
