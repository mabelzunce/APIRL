/**
	\file SiddonProjectorWithAttenuation.cpp
	\brief Archivo que contiene la implementación de la clase SiddonProjector.

	\todo 
	\bug
	\warning
	\author Martín Belzunce (martin.a.belzunce@gmail.com)
	\date 2010.10.06
	\version 1.1.0
*/

#include <Siddon.h>
#include <SiddonProjectorWithAttenuation.h>

SiddonProjectorWithAttenuation::SiddonProjectorWithAttenuation(Image* attImage)
{
  this->numSamplesOnDetector = 1;  
  attenuationMap = new Image(attImage);
}

SiddonProjectorWithAttenuation::SiddonProjectorWithAttenuation(int nSamplesOnDetector, Image* attImage)
{
  this->numSamplesOnDetector = nSamplesOnDetector;  
  attenuationMap = new Image(attImage);
}



/** Sección para Sinogram3D. */
bool SiddonProjectorWithAttenuation::Project (Image* inputImage, Sinogram3D* outputProjection)
{
	return false;
}
// 	Point3D P1, P2;
// 	Line3D LOR;
// 	SiddonSegment** MyWeightsList = (SiddonSegment**)malloc(sizeof(SiddonSegment*));
// 	// Tamaño de la imagen:
// 	SizeImage sizeImage = inputImage->getSize();
// 	// Puntero a los píxeles:
// 	float* ptrPixels = inputImage->getPixelsPtr();
// 	// Inicializo el sino 3D con el que voy a trabajar en cero.
// 	// Lo incializo acá porque despues distintas cobinaciones de z aportan al mismo bin.
// 	outputProjection->FillConstant(0.0);
// 	for(unsigned int i = 0; i < outputProjection->CantSegmentos; i++)
// 	{
// 		printf("Forwardprojection Segmento: %d\n", i);
// 		for(unsigned int j = 0; j < outputProjection->Segmentos[i]->CantSinogramas; j++)
// 		{
// 			printf("\t\tSinograma: %d\n", j);
// 			/// Cálculo de las coordenadas z del sinograma
// 			for(unsigned int m = 0; m < outputProjection->Segmentos[i]->Sinogramas2D[j]->getNumZ(); m++)
// 			{
// 			  int indexRing1 = outputProjection->Segmentos[i]->Sinogramas2D[j]->getRing1FromList(m);
// 			  int indexRing2 = outputProjection->Segmentos[i]->Sinogramas2D[j]->getRing2FromList(m);
// 			  
// 			  for(unsigned int k = 0; k < outputProjection->Segmentos[i]->Sinogramas2D[j]->getNumProj(); k++)
// 			  {
// 			    for(unsigned int l = 0; l < outputProjection->Segmentos[i]->Sinogramas2D[j]->getNumR(); l++)
// 			    {
// 			      /// Cada Sinograma 2D me represnta múltiples LORs, según la mínima y máxima diferencia entre anillos.
// 			      /// Por lo que cada bin me va a sumar cuentas en lors con distintos ejes axiales.
// 			      /// El sinograma de salida lo incializo en cero antes de recorrer los distintos anillos de cada elemento del
// 			      /// sinograma, ya que varias LORS deben aportar al mismo bin del sinograma.
// 
// 			      GetPointsFromLOR(outputProjection->Segmentos[i]->Sinogramas2D[j]->getAngValue(k), outputProjection->Segmentos[i]->Sinogramas2D[j]->getRValue(l), 
// 					      outputProjection->ZValues[indexRing1], outputProjection->ZValues[indexRing2], outputProjection->Rscanner, &P1, &P2);
// 			      LOR.P0 = P1;
// 			      LOR.Vx = P2.X - P1.X;
// 			      LOR.Vy = P2.Y - P1.Y;
// 			      LOR.Vz = P2.Z - P1.Z;
// 			      // Then I look for the intersection between the 3D LOR and the lines that
// 			      // delimits the voxels
// 			      // Siddon					
// 			      unsigned int LengthList;
// 			      Siddon(LOR, inputImage, MyWeightsList, &LengthList,1);
// 			      
// 			      if(LengthList > 0)
// 			      {
// 				      /// Hay elementos dentro del FOV
// 				      for(unsigned int n = 0; n < LengthList; n++)
// 				      {
// 					      // for every element of the systema matrix different from zero,we do
// 					      // the sum(Aij*Xj) for every J
// 					      if((MyWeightsList[0][n].IndexX<sizeImage.nPixelsX)&&(MyWeightsList[0][n].IndexY<sizeImage.nPixelsY)&&(MyWeightsList[0][n].IndexZ<sizeImage.nPixelsZ))
// 					      {		
// 						      outputProjection->Segmentos[i]->Sinogramas2D[j]->incrementSinogramBin(k, l, MyWeightsList[0][n].Segment * 
// 							      ptrPixels[MyWeightsList[0][n].IndexZ*(sizeImage.nPixelsX*sizeImage.nPixelsY)+MyWeightsList[0][n].IndexY * sizeImage.nPixelsX + MyWeightsList[0][n].IndexX]);
// 					      }
// 					      
// 					      /*else
// 					      {
// 						      printf("Siddon fuera\n");
// 					      }*/
// 					      //printf("r:%d phi:%d z1:%d z2:%d x:%d y:%d z:%d w:%f", j, i, indexRing1, indexRing2, MyWeightsList[0][l].IndexX, MyWeightsList[0][l].IndexY, MyWeightsList[0][l].IndexZ, MyWeightsList[0][l].Segment);	
// 				      }
// 				      if(LengthList != 0)
// 				      {
// 					      free(MyWeightsList[0]);
// 				      }
// 				      
// 			      }
// 			    }
// 					// Now I have my estimated projection for LOR i
// 			  }
// 			}
// 		}
// 	}
// 
// 	return true;
// }

bool SiddonProjectorWithAttenuation::Backproject (Sinogram3D* inputProjection, Image* outputImage)
{
	return false;
}
// 	Point3D P1, P2;
// 	Line3D LOR;
// 	SiddonSegment** MyWeightsList = (SiddonSegment**)malloc(sizeof(SiddonSegment*));
// 	// Tamaño de la imagen:
// 	SizeImage sizeImage = outputImage->getSize();
// 	// Puntero a los píxeles:
// 	float* ptrPixels = outputImage->getPixelsPtr();
// 	for(unsigned int i = 0; i < inputProjection->CantSegmentos; i++)
// 	{
// 		printf("Backprojection Segmento: %d\n", i);
// 		for(unsigned int j = 0; j < inputProjection->Segmentos[i]->CantSinogramas; j++)
// 		{
// 			/// Cálculo de las coordenadas z del sinograma
// 			printf("\t\tSinograma: %d\n", j);
// 			for(unsigned int k = 0; k < inputProjection->Segmentos[i]->Sinogramas2D[j]->getNumProj(); k++)
// 			{
// 				for(unsigned int l = 0; l < inputProjection->Segmentos[i]->Sinogramas2D[j]->getNumR(); l++)
// 				{
// 					/// Cada Sinograma 2D me represnta múltiples LORs, según la mínima y máxima diferencia entre anillos.
// 					/// Por lo que cada bin me va a sumar cuentas en lors con distintos ejes axiales.
// 					if(inputProjection->Segmentos[i]->Sinogramas2D[j]->getSinogramBin(k,l) != 0)
// 					{
// 						for(unsigned int m = 0; m < inputProjection->Segmentos[i]->Sinogramas2D[j]->getNumZ(); m++)
// 						{
// 							int indexRing1 = inputProjection->Segmentos[i]->Sinogramas2D[j]->getRing1FromList(m);
// 							int indexRing2 = inputProjection->Segmentos[i]->Sinogramas2D[j]->getRing2FromList(m);
// 							GetPointsFromLOR(inputProjection->Segmentos[i]->Sinogramas2D[j]->getAngValue(k), inputProjection->Segmentos[i]->Sinogramas2D[j]->getRValue(l), 
// 									inputProjection->ZValues[indexRing1], inputProjection->ZValues[indexRing2], inputProjection->Rscanner, &P1, &P2);
// 							LOR.P0 = P1;
// 							LOR.Vx = P2.X - P1.X;
// 							LOR.Vy = P2.Y - P1.Y;
// 							LOR.Vz = P2.Z - P1.Z;
// 							// Then I look for the intersection between the 3D LOR and the lines that
// 							// delimits the voxels
// 							// Siddon					
// 							unsigned int LengthList;
// 							Siddon(LOR, outputImage, MyWeightsList, &LengthList,1);
// 							for(unsigned int n = 0; n < LengthList; n++)
// 							{
// 								// for every element of the systema matrix different from zero,we do
// 								// the sum(Aij*bi/Projected) for every i
// 								if((MyWeightsList[0][n].IndexZ<sizeImage.nPixelsZ)&&(MyWeightsList[0][n].IndexY<sizeImage.nPixelsY)&&(MyWeightsList[0][n].IndexX<sizeImage.nPixelsX))
// 								{
// 									ptrPixels[MyWeightsList[0][n].IndexZ*(sizeImage.nPixelsX*sizeImage.nPixelsY)+MyWeightsList[0][n].IndexY * sizeImage.nPixelsX + MyWeightsList[0][n].IndexX] +=
// 										MyWeightsList[0][n].Segment * inputProjection->Segmentos[i]->Sinogramas2D[j]->getSinogramBin(k,l);	
// 								}
// 								/*else
// 								{
// 									printf("Siddon fuera de mapa\n");
// 								}*/
// 							}
// 							if(LengthList != 0)
// 							{
// 							  /// Solo libero memoria cuando se la pidió, si no hay una excepción.
// 							  free(MyWeightsList[0]);
// 							}
// 						}
// 					}
// 					// Now I have my estimated projection for LOR i
// 				}
// 			}
// 		}
// 	}
// 
// 	return true;
// }

