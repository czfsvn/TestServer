#ifndef __COMMON_20170722_H__
#define __COMMON_20170722_H__

#define SAFE_DELETE(x)  {   if(x) { delete (x); (x) = NULL; }   }
#define SAFE_DELETE_VEC(x)  {   if (x) { delete[](x);   (x) = NULL; }   }

#define SAFE_SUB(x, y)  ((x) > (y) ? (x) - (y) : 0)
#define SAFE_ADD(x, y, max) ((x) + (y) > (max) ? (max) : (x) + (y))

#endif
