#version 330 core
out vec4 FragColor;

uniform vec3 spriteColor;
uniform vec2 screenSize;
uniform vec2 modelSize; // size of ellipse (in global coordinates)
uniform vec2 modelPosition; // position of ellipse (in global coordinates) 
uniform float modelZRotation; // rotation that is applied to the ellipse on the z axis (theta) in radians


//bool PositionIsInCircle(vec2 position, vec2 centre, float radiusX, float radiusY); // non smoothed ellipse code
float GetAlphaOfEllipse(vec2 position, vec2 centre, float radiusX, float radiusY);

float smoothAmount = 0.05; // a decimal (percentage/100) of how much to smooth the ellipse by. Smoothing adds a gradient as a 

void main()
{
	// get the coordinate of the current pixel. gl_FragCoord.xy are screen space (aka global) coordinates but that's ok cos the modelSize and position is too.
	// Note that each coord is offset by 0.5 pixels in x and y
	vec2 pixelPos = gl_FragCoord.xy;

	// get the centre (h,k) of the current ellipse in pixel/global coords. The position of an ellipse is at the bottom-left so add half width and height to get actual centre
	vec2 ellipseCentre = modelPosition + modelSize/2;

	// get the x radius by doing the centreX - bottomLeftPositionX
	// E.g.  if centreX is 5 and position is 2 then the difference between 2 and 5 is 3 or 5 - 3 or centreX - positionX
	float radiusX = ellipseCentre.x - modelPosition.x;

	// same logic then applies to y axis
	float radiusY = ellipseCentre.y - modelPosition.y;
	
	
	// Ok so now we have a pixel position (x,y) and a centre position (h,k) and radius for x-axis (a) and y-axis (b)

	//The only issue is we haven't taken into account rotations.

	// Rotations of a vec2 can be calculated by doing.
	/* 
		I can't use theta sign so ur gonna have to deal with it because encoding isn't supported

		[	x * cos(theta) - y * sin(theta), 
			x * sin(theta) + y * cos(theta)	]  

		The we need to account for the (h,k) a.k.a centre coordinate of the ellipse. You end up getting.

		[	(x-h)* cos(theta) - (y-h) * sin(theta), 
			(x-h)* sin(theta) + (y-h) * cos(theta)		]  

		The normal form of an ellipse is ((x-h)^2)/a^2 + ((y-k)^2)/b^2 = 1

		When we account for rotations we get: (A big ass equation, thank god for desmos)
			Form =	(((x-h) * cos(theta) - (y - k) * sin(theta))^2) / a^2 + (((y-k) * cos(theta) + (x - h) * sin(theta))^2) / b^2 = 1

		 view https://www.desmos.com/calculator/wphy49dcmq and play animation of t value. It's a visualisation I made to figure it out.
		 i don't understand the rotation math that much but see https://www.youtube.com/watch?v=7j5yW5QDC2U&ab_channel=FreyaHolm%C3%A9r
		 and https://learnopengl.com/Getting-started/Transformations for more info. To be fair I'm doing this on a sunday at 10 pm my mind isn't really looking for a big math
		 lesson.

		if you look at that mess and understand it you can actually see we have all of these values

		The x and y value is the pixelPos
		The centre (h,k) is ellipseCentre
		The a is radiusX
		The b is radiusY
		theta is modelZRotation. This is because rotations relative to z axis move everything along the x and y. Anyway basically just accept this is theta ok.

		So the above equation mess I showed youm 
			Form = 1
		This can become an inequality
			Form <= 1

		When this inequality is true you have a pixel that is in the bounds of the ellipse.

		I then do my fancy smoothstep to make it not jagged and actually bleed on the outside to give the illusion of a perfect circle. This is because a square grid 
		don't perfectly fit a mathematically perfect circle yknow. Your screen is a grid of squares (pixels) so yeah.
	*/
	
	// -- smoothing explanation --
	// However, if i were to set the equation to be general equation <= 0.95 I would lose 5% (0.05) of the ellipse's size.
	//	Using this logic I can use glsls smoothstep function where I set an inner ellipse and the outer ellipse is just the original one.
	//	Therefore I can first check if a pixel is in the inner circle, if not then smooth it's value until it reaches out circle.
	//	E.g. if the result of the equation was 0.97 I can use the smoothstep equation where left edge is 0.95 and the right edge is 1. 
	//	From here I get a value of which is 0 if the input is less than 0.95 and then if it is between 0.95 and 1 I get a decimal percentage of how much the input is between them.
	//	If we plug in the values into smoothstep equation ((0.97-0.95)/(1-0.95)) you get 0.02/0.05 = 0.4 = 40%.
	//	Now we can't use this value just yet. If we do 1 - result of smoothstep you get 1 -0.4 = 0.6 = 60%.
	//	Ok now this value is usable. If we set this value to the alpha of the ellipse you end up getting a smoothing effect where the closer 
	//	the result of the ellipse general equation is to 1, the less transparent it will become.
	//	Go onto https://thebookofshaders.com/glossary/?search=smoothstep and type in y = 1.0 - smoothstep(0.95,1.0,x); into the graph section
	//	You can visualise how the closer the input of the general equation is to 1 starting from 0.95, the more it decreases in transparency

	// -- Non smoothed ellipse code --
//	if(! PositionIsInCircle(pixelPos, ellipseCentre, radiusX, radiusY))
//		// don't include pixel in output
//		discard;
		
//	// else just output normally
//	FragColor = vec4(spriteColor, 1.0) ; // set to spriteColor

	// -- smoothed ellipse code --
	FragColor = vec4(spriteColor, GetAlphaOfEllipse(pixelPos, ellipseCentre, radiusX, radiusY)) ; // set to spriteColor and have alpha be the result of smoothing output

} 

// -- Non smoothed ellipse code --
// returns whether ((x-h)^2)/a^2 + ((y-k)^2)/b^2 <= 1 is true
//bool PositionIsInCircle(vec2 position, vec2 centre, float radiusX, float radiusY)	{
//	// just substitute all the values and solve
//	return (pow(position.x - centre.x,2.0) / pow(radiusX,2.0) + pow(position.y - centre.y,2.0) / pow(radiusY,2.0) <= 1.0);
//}

// returns a value from 0 to 1 which can be set to the alpha value of fragment shader output. This function smooths the circle to get a nicer output
float GetAlphaOfEllipse(vec2 position, vec2 centre, float radiusX, float radiusY)	{
	// just substitute all the values and solve
	// old equation
	//	float result = pow(position.x - centre.x,2.0) / pow(radiusX,2.0) + pow(position.y - centre.y,2.0) / pow(radiusY,2.0);
	// new equation
	// (((x-h) * cos(theta) - (y - k) * sin(theta))^2) / a^2 + (((y-k) * cos(theta) + (x - h) * sin(theta))^2) / b^2 = 1
	float result = 
	// ((x-h) * cos(theta) - (y - k) * sin(theta)) ^ 2
	pow((position.x - centre.x) * cos(modelZRotation) - (position.y - centre.y) * sin(modelZRotation),2.0) 
	/ 
	pow(radiusX,2.0) // a^2
	+ 
	// ((y-k) * cos(theta) + (x - h) * sin(theta)) ^ 2
	pow((position.y - centre.y) * cos(modelZRotation) + (position.x - centre.x) * sin(modelZRotation),2.0)
	/ 
	pow(radiusY,2.0); // b^2

	// apply smoothing to the result
	float smoothedResult = 1-smoothstep(1.0 - smoothAmount ,1, result);

	// if the value is within ellipse
	if(smoothedResult <= 1.0)
		// 
		return smoothedResult;
	else
		// discard fragment
		discard;
}