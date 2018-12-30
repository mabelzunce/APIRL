/**
	\file SiddonProjector.h
	\brief Definition file for the class SiddonProjector.

	SiddonProjector is a derivate class from abstract class Projector. It defines and implement forward and back
	projection operations for different type of datasets using the Siddon algorithm.
	
	
	\todo Translate comments.
	\bug
	\warning
	\author Martín Belzunce (martin.a.belzunce@gmail.com)
*/

#ifndef _SIDDONPROJECTOR_H
#define _SIDDONPROJECTOR_H

#include <Sinogram3D.h>
#include <Projector.h> 
#include <Images.h>

class DLLEXPORT SiddonProjector : virtual Projector
{
  private:
	/// Número de lors utilizadas en el siddon por bin del sinograma.
	/** Número de lors utilizadas en el siddon por bin del sinograma. Por default es una lor sola
		que sale del centro del detector. Si se configuraran más son n líneas paralelas equiespaciadas
		sobre el detector por cada bin del sinograma.
		*/
	int numSamplesOnDetector;
	/// Número de lors utilizadas axialmente en por bin del sinograma.
	/** Número de lors utilizadas axialmente en por bin del sinograma. Por default es una lor sola
		que sale del centro del anillo. Si se configuraran más son n líneas paralelas equiespaciadas
		sobre ring correpsondiente a ese sinograma.
		*/
	int numAxialSamplesOnDetector;
  public:
	/// Constructor base. 
	/** El cosntructor base setea una lor por bin. */
	SiddonProjector();
	/** Este constructor setea la cantidad de lors por bin que se desea utilizar. */
	SiddonProjector(int nSamplesOnDetector);
	/** Este constructor setea la cantidad de lors por bin que se desea utilizar. */
	SiddonProjector(int nSamplesOnDetector, int nAxialSamplesOnDetector);
	/** Backprojection con Siddon para Sinogram2D. */
	bool Backproject (Sinogram2D* InputSinogram, Image* outputImage);  
	/** DivideAndBackprojection con Siddon para Sinogram2D. */
	bool DivideAndBackproject (Sinogram2D* InputSinogram, Sinogram2D* EstimatedSinogram, Image* outputImage);
	/** Projection con Siddon para Sinogram2D. */
	bool Project(Image* image, Sinogram2D* projection);
	
	/** Backprojection con Siddon para Sinogram3D. */
	bool Backproject (Sinogram3D* InputSinogram, Image* outputImage); 
	/** DivideAndBackprojection con Siddon para Sinogram3D. */
	bool DivideAndBackproject (Sinogram3D* InputSinogram, Sinogram3D* EstimatedSinogram, Image* outputImage);
	/** Projection con Siddon para Sinogram3D. */
	bool Project(Image* image, Sinogram3D* projection);
};

#endif // PROJECTOR_H
