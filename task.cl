// Sampler
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
								CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void task (read_only image2d_t inputImage, write_only image2d_t outputImage1, 
							write_only image2d_t outputImage2, write_only image2d_t outputImage3)
{
	// Get image dimensions
	unsigned int imageWidth = get_global_size(0) - 1;
	unsigned int imageHeight = get_global_size(1) - 1;

	// Get pixel coordinates
	int2 coord = (int2)(get_global_id(0),  get_global_id(1));

	// Get pixel from input
	float4 pixel = read_imagef(inputImage, sampler, coord);

	// ----------------------- TASK A ---------------------------------------

	// Flip coordinates horizontally
	int2 newCoord = (int2)((imageWidth - coord.x), coord.y);

	// Output pixel
	write_imagef(outputImage1, newCoord, pixel);

	// ----------------------- TASK B ---------------------------------------

	// Flip coordinates vertically
	newCoord = (int2)(coord.x, (imageHeight - coord.y));
	write_imagef(outputImage2, newCoord, pixel);

	// ----------------------- TASK C ---------------------------------------
	
	// Flip coordinates both horizontally and vertically
	newCoord = (int2)((imageWidth - coord.x), (imageHeight - coord.y));
	write_imagef(outputImage3, newCoord, pixel);

}