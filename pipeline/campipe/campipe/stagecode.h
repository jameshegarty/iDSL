//
//  stagecode.h
//  campipe
//
//  Created by Phaedon Sinis on 2/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef campipe_stagecode_h
#define campipe_stagecode_h


// Saves output frames to disk. Not a real pipeline stage, but
// useful for the simulation.
void *finalstage (void *args);

// Unary color transformation. Just a simple experiment.
void *colordarkenstage (void *args);

// Dumb image merge, to test stage with 2 images
void *sillymerge (void *args);


#endif
