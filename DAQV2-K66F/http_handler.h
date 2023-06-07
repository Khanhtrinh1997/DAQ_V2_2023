#include "Header.h"

//Forward declaration of functions
error_t httpServerCgiCallback(HttpConnection *connection,
                              const char_t *param);

error_t httpServerUriNotFoundCallback(HttpConnection *connection,
                                      const char_t *uri);

error_t httpServerRequestCallback(HttpConnection *connection,
                                  const char_t *request);
void http_handler_init(void);
