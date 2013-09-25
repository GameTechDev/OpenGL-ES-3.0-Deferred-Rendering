/*! @file ViewController.m
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#import "ViewController.h"

#include "game.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
@interface ViewController () {
    Game*   _game;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController

- (void)viewDidLoad
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

    self.preferredFramesPerSecond = 60;

    [self setupGL];
}

- (void)dealloc
{
    [self tearDownGL];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;

        [self tearDownGL];

        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

    _game = create_game((int)(self.view.frame.size.width*self.view.contentScaleFactor),
                        (int)(self.view.frame.size.height*self.view.contentScaleFactor));
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    destroy_game(_game);
}

- (void)update
{
    update_game(_game);
}
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    render_game(_game);

    (void)sizeof(view);
    (void)sizeof(rect);
}

@end
