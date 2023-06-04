/* Copyright (c) 2016-2023, The C++ IPFS client library developers

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include <ipfs/http/transport-curl.h>

#include <cassert>
#include <sstream>

int main(int, char**) {
  {
    /* test normal fetch */
    ipfs::http::TransportCurl transportCurl(false);
    {
      std::stringstream response;
      transportCurl.Fetch("https://example.com/", {}, &response);
      assert(!response.str().empty());
    }
    /* test move constructor */
    {
      ipfs::http::TransportCurl transportCurl2(std::move(transportCurl));
      std::stringstream response;
      transportCurl2.Fetch("https://example.com/", {}, &response);
      assert(!response.str().empty());
    }
  }
  /* test move assignment to other object */
  {
    ipfs::http::TransportCurl transportCurl(false);
    ipfs::http::TransportCurl transportCurl2(false);
    transportCurl2 = std::move(transportCurl);
    std::stringstream response;
    transportCurl2.Fetch("https://example.com/", {}, &response);
    assert(!response.str().empty());
  }
}