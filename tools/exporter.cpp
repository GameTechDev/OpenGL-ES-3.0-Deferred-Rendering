#include "../src/vec_math.h"
#include "../src/assert.h"
extern "C" {
#include "../src/utility.h"
#include "../src/scene.h"
}
#include <stdlib.h>
#include <stddef.h>
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <sstream>

/* Constants
 */

/* Types
 */

/* Variables
 */
 
/* Internal functions
 */

/* External functions
 */
int main(int argc, const char *argv[])
{
    for(int ii=1; ii<argc;++ii) {
        SceneData* scene = _load_scene_data(argv[ii]);
    }
    return 0;
}
