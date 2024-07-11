
#import "PrintApplication.h"

@implementation PrintApplication

- run

{
	BOOL flag=YES;
	while (flag)
	{
		switch([[PrintPanel new] runModal])
		{
			case NX_CANCELTAG:
				printf("0");
				flag=NO;
				break;
			case NX_OKTAG:
				printf("1");
				flag=NO;
				break;
			case NX_SAVETAG:
				printf("2%s", [[SavePanel new] filename]);
				flag=NO;
				break;
			case NX_FAXTAG:
				NXRunAlertPanel("Error",
					"CGraph does not support Faxing",
					"Okay", NULL, NULL); 
				break;
			case NX_PREVIEWTAG:
				NXRunAlertPanel("Error",
					"Presumably, you can already see the document",
					"Okay", NULL, NULL); 
				break;
		}
	}
	return self;
}




@end
