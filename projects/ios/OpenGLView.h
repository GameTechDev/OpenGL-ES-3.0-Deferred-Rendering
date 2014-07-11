#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface OpenGLView : GLKViewController

@property(strong, nonatomic) EAGLContext* context;
@property struct Game* game;

@end
