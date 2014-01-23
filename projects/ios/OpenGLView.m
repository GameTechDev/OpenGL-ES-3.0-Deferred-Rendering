/* Copyright (c) <2013>, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#import "OpenGLView.h"
#include "game.h"

@implementation OpenGLView

-(void)viewDidLoad
{
    GLKView *view = nil;

    [super viewDidLoad];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

    if (!self.context) {
        NSLog(@"Failed to create ES 3.0 context");
        self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!self.context) {
            NSLog(@"Failed to create ES 2.0 context");
        }
    }

    view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.multipleTouchEnabled = YES;
    view.contentScaleFactor = [[UIScreen mainScreen] scale];

    self.preferredFramesPerSecond = 60;
    
    [self setupGL];

    self.game = create_game();
}
- (void)dealloc
{
    [self tearDownGL];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}
-(void)viewDidLayoutSubviews
{
    int width = (int)[self screenSize].size.width;
    int height = (int)[self screenSize].size.height;
    resize_game(self.game, width, height);
}
-(void)update
{
    update_game(self.game);
}
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    render_game(self.game);

    (void)sizeof(view);
    (void)sizeof(rect);
}
-(CGRect)screenSize
{
    CGRect rect = [[UIScreen mainScreen] bounds];
    rect.size.height *= [self deviceScale];
    rect.size.width *= [self deviceScale];

    if (UIDeviceOrientationIsLandscape(self.interfaceOrientation)) {
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
