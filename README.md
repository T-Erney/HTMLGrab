# HTMLGrab

HTMLGrab is a test project for creating a subset clone for applications such as
curl and wget. This proof of concept only works for IPv4 on port 80 for HTTP 
requests and reponses. Currently, the htmlgrab function only returns the entire
contents of the HTTP request consisting of the HTTP header and HTML source. 

Any future work for this project would include:
- Creating requests for HTTPS (port 443)
- Creating and utilizing sockets for IPv6
- Displaying progress for downloads
  - in the form of a progress bar 
- Parsing the HTML header 
  - possibly into a hashmap for quick look-ups for status code and html source
  size

The project is currently written as a standalone static header. If you for some
reason desire to create your own implementation and not use the implementations 
provided from the header, you can use macros to not define the functions 
declared in the header. The following will allow you to define your own 
implementations while using the same header:

```c
#define HTMLGRAB_NOBUILD
#include "htmlgrab.h"
```

## Usage 

```c
#include "htmlgrab.h"
#include <stdio.h>

int main(void) {
  char* html = htmlgrab("www.google.com");
  printf("%s\n", html);

  return 0;
}
```

## License 
[MIT](https://www.choosealicense.com/licenses/mit/)
