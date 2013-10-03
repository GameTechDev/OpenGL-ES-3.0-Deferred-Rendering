/*! @file OpenGLView.h
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface OpenGLView : GLKViewController

@property(strong, nonatomic) EAGLContext* context;
@property struct Game* game;

@end
