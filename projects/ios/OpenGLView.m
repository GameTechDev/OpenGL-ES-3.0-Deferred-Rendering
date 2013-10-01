/*! @file OpenGLView.m
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#import "OpenGLView.h"

@implementation OpenGLView

-(void)viewDidLoad
{
    GLKView *view = nil;

    [super viewDidLoad];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }

    view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.multipleTouchEnabled = YES;
    view.contentScaleFactor = [[UIScreen mainScreen] scale];

    self.preferredFramesPerSecond = 60;

    [self setupGL];
    [self setupDisplayLink];
}
- (void)dealloc
{
    [self tearDownGL];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)setupDisplayLink {
    CADisplayLink* display_link = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}
-(void)render:(CADisplayLink*)display_link {
    // update();
    // render();
    glClear(GL_COLOR_BUFFER_BIT);
    [self.context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    glClearColor(1.0, 0.0f, 1.0f, 1.0f);
}
- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

@end
