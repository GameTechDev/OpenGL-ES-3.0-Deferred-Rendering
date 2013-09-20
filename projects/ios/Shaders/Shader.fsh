//
//  Shader.fsh
//  deferred gles
//
//  Created by Kyle Weicht on 9/19/13.
//  Copyright (c) 2013 Intel. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
