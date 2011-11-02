/* 
    Measure latency of IPC using unix domain sockets

    Copyright (c) 2010 Erik Rigtorp <erik@rigtorp.com>
    Copyright (c) 2011 Anil Madhavapeddy <anil@recoil.org>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <inttypes.h>
#include <err.h>

#include "xutil.h"

int
main(int argc, char *argv[])
{
  int sv[2]; /* the pair of socket descriptors */
  int size;
  char *buf;
  int64_t count, i;
  bool per_iter_timings;

  parse_args(argc, argv, &per_iter_timings, &size, &count);

  buf = xmalloc(size);

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1)
    err(1, "socketpair");

  if (!fork()) {  /* child */
    for (i = 0; i < count; i++) {
      xread(sv[1], buf, size);
      xwrite(sv[1], buf, size);
    }
  } else { /* parent */
    latency_test(
		 do {
		   xwrite(sv[0], buf, size);
		   xread(sv[0], buf, size);
		 } while (0),
		 per_iter_timings,
		 count);
  }
  return 0;
}
