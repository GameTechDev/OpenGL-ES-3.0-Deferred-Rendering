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
    view.multipleTouchEnabled = YES;

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
-(void)viewDidLayoutSubviews
{
    int width = (int)(self.view.frame.size.width*self.view.contentScaleFactor);
    int height = (int)(self.view.frame.size.height*self.view.contentScaleFactor);
    
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    if(UIDeviceOrientationIsLandscape(orientation)) {
        resize_game( _game, height, width);
    } else {
        resize_game( _game, width, height);
    }
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

-(float)deviceScale
{
    if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] &&
        ([UIScreen mainScreen].scale)) {
        return (float)[UIScreen mainScreen].scale;
    }
    return 1.0f;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    TouchPoint points[16] = {0};
    int num_points = 0;
    for(UITouch *touch in touches) {
        CGPoint tapPoint = [touch locationInView:nil];
        tapPoint.x *= [self deviceScale];
        tapPoint.y *= [self deviceScale];
        if([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
            // The OS should really rotate the taps for us :-(
            tapPoint.y = [self deviceScale] - tapPoint.y;
            tapPoint.x = [self deviceScale] - tapPoint.x;
        }
        points[num_points].pos.x = (float)tapPoint.x;
        points[num_points].pos.y = (float)tapPoint.y;
        points[num_points].index = (intptr_t)touch;
        num_points++;
    }
    add_touch_points(_game, num_points, points);
    (void)sizeof(event);
}
 
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    TouchPoint points[16] = {0};
    int num_points = 0;
    for(UITouch *touch in touches) {
        CGPoint tapPoint = [touch locationInView:nil];
        tapPoint.x *= [self deviceScale];
        tapPoint.y *= [self deviceScale];
        if([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
            // The OS should really rotate the taps for us :-(
            tapPoint.y = [self deviceScale] - tapPoint.y;
            tapPoint.x = [self deviceScale] - tapPoint.x;
        }
        points[num_points].pos.x = (float)tapPoint.x;
        points[num_points].pos.y = (float)tapPoint.y;
        points[num_points].index = (intptr_t)touch;
        num_points++;
    }
    update_touch_points(_game, num_points, points);
    (void)sizeof(event);
}
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    TouchPoint points[16] = {0};
    int num_points = 0;
    for(UITouch *touch in touches) {
        CGPoint tapPoint = [touch locationInView:nil];
        tapPoint.x *= [self deviceScale];
        tapPoint.y *= [self deviceScale];
        if([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
            // The OS should really rotate the taps for us :-(
            tapPoint.y = [self deviceScale] - tapPoint.y;
            tapPoint.x = [self deviceScale] - tapPoint.x;
        }
        points[num_points].pos.x = (float)tapPoint.x;
        points[num_points].pos.y = (float)tapPoint.y;
        points[num_points].index = (intptr_t)touch;
        num_points++;
    }
    remove_touch_points(_game, num_points, points);
    (void)sizeof(event);
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [self touchesEnded:touches withEvent:event];
}

@end
