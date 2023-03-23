# Changelog
Date format is Australian so (dd/mm/yyyy)
## Project started
Date - 6/3/2023
## V 0.1 Repo created and everything added
Date - 18/3/2023 
* Added 
    * Scene class (updates every frame)
    * Resource manager class
    * Transform class
    * Texture2D class
    * Shader program class
    * Orthographic camera class
    * Event system
    * Entity class  

Currently the renderer is quite basic but it has a frame loop that renders entities under a scene and has a working camera. All of the added classes work properly and have a decent amount of functionality.
## V 0.1.1 Rectangle renderer
Date - 18/3/2023 9:23 PM
* Added 
   * Rectangle renderer class
   * Test rectangle in main which outputs to screen  
* Changes   
   * Cleaned up some commented code that wasn't needed  
## V 0.1.2 Ellipse renderer
Date - 19/3/2023 8:30 PM
* Added 
   * Ellipse renderer class
   * Test ellipse in main which outputs to screen  
   * Ellipse fragment and vertex shaders
   * Anti aliasing (MSAA)
   * vec2 uniform utility function
   * Enum for ellipse renderer
* Changes   
   * Made component derived classes use OrthoCamera instead of Camera because this is a 2D renderer not a 3D one.  
## V 0.1.3 Implemented proper support and system to deal with depth and blending (transparency)
Date - 23/3/23 11:30 pm
* Added
   * hasTransparency property for all components with getters and setters functions.
   * Alpha (transparency) property for ellipse,circle and rect renderer classes which can be acessed through a getter and setter function.
* Changed
   * Transform position vector is now a vec2 (used to be vec3)
   * In fragment shaders, color value is passed as a vec4 where the 4th value is alpha
   * Entity names now have getter and setter functions, the name updates in the scene whenever you change it
   * When transform converts position to matrix, for zIndex it does highestZIndex in scene - 1 or just 0 if not in a scene
   * Test objects in the main function
   * Ortho camera far plane defaults to 100
   * Entities are now put into two map. One is sorted entities, the other being any with transparency. 
   * There is also two vectors that are used, both with correlating indexes, that give you each transparent entity in scene sorted by zIndex (ascending). This is used to render the scene from the furthest objects from camera to last
   * Oh yeah, also implemented zIndex property for entities. It is under an entity's transform.
 * Other notes
   * I hate english (as a subject)
