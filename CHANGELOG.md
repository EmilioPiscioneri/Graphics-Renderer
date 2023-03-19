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
