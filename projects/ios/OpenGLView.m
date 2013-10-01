/*! @file OpenGLView.m
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#import "OpenGLView.h"
#include "game.h"

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

    self.game = create_game([self screenSize].size.width, [self screenSize].size.height);
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
-(CGRect)screenSize
{
    CGRect rect = [[UIScreen mainScreen] bounds];
    rect.size.height *= [self deviceScale];
    rect.size.width *= [self deviceScale];

    if ([[UIDevice currentDevice] orientation] == UIInterfaceOrientationLandscapeLeft ||
        [[UIDevice currentDevice] orientation] == UIInterfaceOrientationLandscapeRight) {
        CGRect temp = rect;
        rect.size.width = temp.size.height;
        rect.size.height = temp.size.width;
    }
    return rect;
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

-(float)deviceScale
{
    if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] &&
        ([UIScreen mainScreen].scale)) {
        return (float)[UIScreen mainScreen].scale;
    }
    return 1.0f;
}
- (int)prepareTouches:(NSSet*)touches withPoints:(TouchPoint*)points
{
    CGRect bounds = [self screenSize];
    int num_points = 0;
    for(UITouch *touch in touches) {
        CGPoint tapPoint = [touch locationInView:nil];
        tapPoint.x *= [self deviceScale];
        tapPoint.y *= [self deviceScale];
        
        if ([[UIDevice currentDevice] orientation] == UIInterfaceOrientationLandscapeLeft) {
            CGPoint temp = tapPoint;
            tapPoint.x = bounds.size.width - temp.y;
            tapPoint.y = temp.x;
        } else if ([[UIDevice currentDevice] orientation] == UIInterfaceOrientationLandscapeRight) {
            CGPoint temp = tapPoint;
            tapPoint.x = temp.y;
            tapPoint.y = bounds.size.height - temp.x;
        } else if([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
            // The OS should really rotate the taps for us :-(
            tapPoint.y = bounds.size.height - tapPoint.y;
            tapPoint.x = bounds.size.width - tapPoint.x;
        }
        points[num_points].pos.x = (float)tapPoint.x;
        points[num_points].pos.y = (float)tapPoint.y;
        points[num_points].index = (intptr_t)touch;
        num_points++;
    }
    return num_points;
}
-(NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}
-(BOOL)shouldAutorotate
{
    return YES;
}
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    TouchPoint points[16] = {0};
    int num_points = [self prepareTouches:touches withPoints:points];
    add_touch_points(_game, num_points, points);
    (void)sizeof(event);
}
 
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    TouchPoint points[16] = {0};
    int num_points = [self prepareTouches:touches withPoints:points];
    update_touch_points(_game, num_points, points);
    (void)sizeof(event);
}
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    TouchPoint points[16] = {0};
    int num_points = [self prepareTouches:touches withPoints:points];
    remove_touch_points(_game, num_points, points);
    (void)sizeof(event);
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [self touchesEnded:touches withEvent:event];
}

@end
