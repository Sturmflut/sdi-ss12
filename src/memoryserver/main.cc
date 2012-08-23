#include <l4/sigma0.h>
#include <l4io.h>

#include <sdi/sdi.h>
#include <sdi/constants.h>

int main(void)
{
	/* Get all available pages */
	int pagecount = 0;
	L4_Fpage_t page;
	L4_ThreadId_t sigma0id = L4_GlobalId ( 48, 1);
	
	printf("memsvr getting pages from %lx\n", sigma0id.raw);

	do {
		page = L4_Sigma0_GetPage (sigma0id, L4_Fpage (-1, 4*1024*1024));

		if(!L4_IsNilFpage (page))
		{
//		    printf("Got page %lx\n", L4_Address(page));
		    pagecount++;
		}
	}while(!(L4_IsNilFpage (page)));

	printf("Got %i pages of size %i from sigma0\n", pagecount, 4*1024*1024);

	printf("memoryserver Pager: %lx\n", L4_Pager().raw);

	while(42)
	{
	}
}

