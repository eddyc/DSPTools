//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import "___FILEBASENAME___.h"

___FILEBASENAME___ *___FILEBASENAME____new()
{
    ___FILEBASENAME___ *self = calloc(1, sizeof(___FILEBASENAME___));
    return self;
}

void ___FILEBASENAME____delete(___FILEBASENAME___ *self)
{
    free(self);
    self = NULL;
}