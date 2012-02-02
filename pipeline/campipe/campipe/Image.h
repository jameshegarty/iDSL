//
//  Image.h
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef campipe_Image_h
#define campipe_Image_h



struct Image {

    // metadata
    const char *stageName;  
    char *frameName;
    
    // image buffer (or pointer to a BMP struct or whatever)
    // TODO
    unsigned char *data;
    
    int width, height, channels;
};

#endif
