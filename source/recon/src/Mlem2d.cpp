
#include <Mlem2d.h>


Mlem2d::Mlem2d(Sinogram2D* cInputProjection, Image* cInitialEstimate, string cPathSalida, string cOutputPrefix, int cNumIterations, int cSaveIterationInterval, bool cSaveIntermediate, bool cSensitivityImageFromFile, Projector* cForwardprojector, Projector* cBackprojector) : Mlem(cInitialEstimate, cPathSalida, cOutputPrefix, cNumIterations, cSaveIterationInterval, cSaveIntermediate, cSensitivityImageFromFile, cForwardprojector,  cBackprojector)
{
  inputProjection = cInputProjection;
  // Inicializo puntero con 1 byte para luego hacer el realloc cuando sea necesario:
  this->likelihoodValues = (float*)malloc(sizeof(float)*1);
  
  // Para la reconstrucción 2d generérica sin scanner en particular, el fov es el ed la imagen:
  inputProjection->setRadioFov_mm(this->initialEstimate->getFovRadio());
  //inputProjection->setRadioFov_mm(280);
}

Mlem2d::Mlem2d(string configFilename)
{
  /// Inicializo las variables con sus valores por default
  // Inicializo puntero con 1 byte para luego hacer el realloc cuando sea necesario:
  this->likelihoodValues = (float*)malloc(sizeof(float)*1);
}

bool Mlem2d::setMultiplicativeProjection(string multiplicativeFilename)
{
  // Problema porque sinogram2d no tiene get lor para hacer backprojection
  // Instead of create a new sinogram instantiating a new object, I copy from inputProject. This way, is independent of the type
  // of derived class od sinogram3d it is being used.
  multiplicativeProjection = inputProjection->Copy();
  multiplicativeProjection->readFromInterfile((char*)multiplicativeFilename.c_str());
  enableMultiplicativeTerm = true;
  return enableMultiplicativeTerm;
}

bool Mlem2d::setMultiplicativeProjection(Sinogram2D* multiplicative)
{
  multiplicativeProjection = multiplicative->Copy();
  enableMultiplicativeTerm = true;
  return enableMultiplicativeTerm;
}

/// Método que carga un sinograma desde un archivo interfile con la estimación de scatter para aplicar como corrección.
bool Mlem2d::setAdditiveProjection(string additiveFilename)
{
  // Instead of create a new sinogram instantiating a new object, I copy from inputProject. This way, is independent of the type
  // of derived class od sinogram3d it is being used.
  additiveProjection = inputProjection->Copy();
  additiveProjection->readFromInterfile((char*)additiveFilename.c_str());
  enableAdditiveTerm = true;
  return enableAdditiveTerm;
}

bool Mlem2d::setAdditiveProjection(Sinogram2D* additive)
{
  additiveProjection = additive->Copy();
  enableAdditiveTerm = true;
  return enableAdditiveTerm;
}

/// Método público que realiza la reconstrucción en base a los parámetros pasados al objeto Mlem instanciado
bool Mlem2d::Reconstruct()
{
  /// Tamaño de la imagen:
  SizeImage sizeImage = reconstructionImage->getSize();
  /// Proyección auxiliar, donde guardo el sinograma proyectado:
  Sinogram2D* estimatedProjection = inputProjection->Copy();
  estimatedProjection->FillConstant(0);
  /// Imagen donde guardo la backprojection.
  Image* backprojectedImage = new Image(reconstructionImage->getSize());
  /// Puntero a la imagen.
  float* ptrPixels = reconstructionImage->getPixelsPtr();
  /// Puntero a la sensitivity image.
  float* ptrSensitivityPixels = sensitivityImage->getPixelsPtr();
  /// Puntero a la sensitivity image.
  float* ptrBackprojectedPixels = backprojectedImage->getPixelsPtr();
  /// Puntero del array con los tiempos de reconstrucción por iteración.
  float* timesIteration_mseg;
  /// Puntero del array con los tiempos de backprojection por iteración.
  float* timesBackprojection_mseg;
  /// Puntero del array con los tiempos de forwardprojection por iteración.
  float* timesForwardprojection_mseg;
  /// Puntero del array con los tiempos de pixel update por iteración.
  float* timesPixelUpdate_mseg;
  /// String de c para utlizar en los mensajes de logueo.
  char c_string[512];
  /// Pido memoria para los arrays, que deben tener tantos elementos como iteraciones:
  timesIteration_mseg = (float*)malloc(sizeof(float)*this->numIterations);
  timesBackprojection_mseg = (float*)malloc(sizeof(float)*this->numIterations);
  timesForwardprojection_mseg = (float*)malloc(sizeof(float)*this->numIterations);
  timesPixelUpdate_mseg = (float*)malloc(sizeof(float)*this->numIterations);
  /// El vector de likelihood Ya había sido alocado previamente por lo que uso realloc. Tiene
  /// un elemento más porque el likelihood es previo a la actualización de la imagen, o sea que inicia
  /// con el initialEstimate y termina con la imagen reconstruida.
  this->likelihoodValues = (float*)realloc(this->likelihoodValues, sizeof(float)*(this->numIterations + 1));

  /// Me fijo si la sensitivity image la tengo que cargar desde archivo o calcularla
  if(sensitivityImageFromFile)
  {
    /// Leo el sensitivity volume desde el archivo
    sensitivityImage->readFromInterfile((char*) sensitivityFilename.c_str());
    ptrSensitivityPixels = sensitivityImage->getPixelsPtr();
    updateUpdateThreshold();
  }
  else
  {
    /// Calculo el sensitivity volume
    if(computeSensitivity(sensitivityImage)==false)
    {
      strError = "Error al calcular la sensitivity Image.";
      return false;
    }
    // La guardo en disco.
    string sensitivityFileName = outputFilenamePrefix;
    sensitivityFileName.append("_sensitivity");
    sensitivityImage->writeInterfile((char*)sensitivityFileName.c_str());
  }
  /// Inicializo el volumen a reconstruir con la imagen del initial estimate:
  reconstructionImage = new Image(initialEstimate);
  ptrPixels = reconstructionImage->getPixelsPtr();
  /// Hago el log de la reconstrucción:
  Logger* logger = new Logger(logFileName);
  /// Escribo el título y luego los distintos parámetros de la reconstrucción:
  logger->writeLine("######## ML-EM Reconstruction #########");
  logger->writeValue("Name", this->outputFilenamePrefix);
  logger->writeValue("Type", "ML-EM");
  sprintf(c_string, "%d", this->numIterations);
  logger->writeValue("Iterations", c_string);
  logger->writeValue("Input Projections", "2D Sinogram");
  sprintf(c_string, "%d[r] x %d[ang]", estimatedProjection->getNumR(), estimatedProjection->getNumProj());
  logger->writeValue("Size of Sinogram2D",c_string);
  sprintf(c_string, "%d", sizeReconImage.nDimensions);
  logger->writeValue("Image Dimensions", c_string);
  sprintf(c_string, "%d[x] x %d[y] x %d[z]", this->sizeReconImage.nPixelsX, this->sizeReconImage.nPixelsY, this->sizeReconImage.nPixelsZ);
  logger->writeValue("Image Size", c_string);

  // También se realiza un registro de los tiempos de ejecución:
  clock_t initialClock = clock();
  //Start with the iteration
  printf("Iniciando Reconstrucción...\n");
  /// Arranco con el log de los resultados:
  strcpy(c_string, "_______RECONSTRUCCION_______");
  logger->writeLine(c_string, (int)strlen(c_string));
  /// Voy generando mensajes con los archivos creados en el log de salida:

  int nPixels = reconstructionImage->getPixelCount();
  //updateThreshold = 0;
  for(int t = 0; t < this->numIterations; t++)
  {
    clock_t initialClockIteration = clock();
    printf("Iteración Nº: %d\n", t);
    /// Proyección de la imagen:
    forwardprojector->Project(reconstructionImage, estimatedProjection);
    clock_t finalClockProjection = clock();
    /// Only appply additive sinogram because the multiplicative is in the sensitivity image:
    if(enableAdditiveTerm)
      estimatedProjection->addBinToBin(additiveProjection);
    /// Guardo el likelihood (Siempre va una iteración atrás, ya que el likelihhod se calcula a partir de la proyección
    /// estimada, que es el primer paso del algoritmo). Se lo calculo al sinograma
    /// proyectado, respecto del de entrada.
    this->likelihoodValues[t] = estimatedProjection->getLikelihoodValue(inputProjection);
    
    /// Pongo en cero la proyección estimada, y hago la backprojection.
    backprojectedImage->fillConstant(0);
    /// Divido input sinogram por el estimated:
    estimatedProjection->inverseDivideBinToBin(inputProjection);

    /// Retroproyecto
    backprojector->Backproject(estimatedProjection, backprojectedImage);
    ///backprojector->DivideAndBackproject(inputProjection, estimatedProjection, backprojectedImage);
    clock_t finalClockBackprojection = clock();
    /// Actualización del Pixel
    for(int k = 0; k < nPixels; k++)
    {
      /// Si el coeficiente de sensitivity es menor que 1 puedo, plantear distintas alternativas, pero algo
      /// hay que hacer sino el valor del píxel tiende a crecer demasiado. .
      if(ptrSensitivityPixels[k]>updateThreshold)
      {
	ptrPixels[k] = ptrPixels[k] * ptrBackprojectedPixels[k] / ptrSensitivityPixels[k];
      }
      else
      {
	/// Si la sensitivity image es distinta de cero significa que estoy fuera del fov de reconstrucción
	/// por lo que pongo en cero dicho píxel:
	ptrPixels[k] = 0;
      }
    }
    /// Verifico
    if(saveIterationInterval != 0)
    {
      if((t%saveIterationInterval)==0)
      {
	sprintf(c_string, "%s_iter_%d", outputFilenamePrefix.c_str(), t); /// La extensión se le agrega en write interfile.
	string outputFilename;
	outputFilename.assign(c_string);
	reconstructionImage->writeInterfile((char*)outputFilename.c_str());
	/// Termino con el log de los resultados:
	sprintf(c_string, "Imagen de iteración %d guardada en: %s", t, outputFilename.c_str());
	logger->writeLine(c_string);
	if(saveIntermediateProjectionAndBackprojectedImage)
	{
	  // Tengo que guardar la estimated projection, y la backprojected image.
	  sprintf(c_string, "%s_projection_iter_%d", outputFilenamePrefix.c_str(), t); /// La extensión se le agrega en write interfile.
	  outputFilename.assign(c_string);
	  estimatedProjection->writeInterfile((char*)outputFilename.c_str());
	  sprintf(c_string, "%s_backprojected_iter_%d", outputFilenamePrefix.c_str(), t); /// La extensión se le agrega en write interfile.
	  outputFilename.assign(c_string);
	  backprojectedImage->writeInterfile((char*)outputFilename.c_str());
	}
      }
    }
    clock_t finalClockIteration = clock();
    /// Cargo los tiempos:
    timesIteration_mseg[t] = (float)(finalClockIteration-initialClockIteration)*1000/(float)CLOCKS_PER_SEC;
    timesBackprojection_mseg[t] = (float)(finalClockBackprojection-finalClockProjection)*1000/(float)CLOCKS_PER_SEC;
    timesForwardprojection_mseg[t] = (float)(finalClockProjection-initialClockIteration)*1000/(float)CLOCKS_PER_SEC;
    timesPixelUpdate_mseg[t] = (float)(finalClockIteration-finalClockBackprojection)*1000/(float)CLOCKS_PER_SEC;
  }

  clock_t finalClock = clock();
  sprintf(c_string, "%s_final", outputFilenamePrefix.c_str()); /// La extensión se le agrega en write interfile.
  string outputFilename;
  outputFilename.assign(c_string);
  reconstructionImage->writeInterfile((char*)outputFilename.c_str());
  /// Termino con el log de los resultados:
  sprintf(c_string, "Imagen final guardada en: %s", outputFilename.c_str());
  logger->writeLine(c_string);
  /// Calculo la proyección de la última imagen para poder calcular el likelihood final:
  //forwardprojector->Project(reconstructionImage, estimatedProjection);
  this->likelihoodValues[this->numIterations] = estimatedProjection->getLikelihoodValue(inputProjection);

  float tiempoTotal = (float)(finalClock - initialClock)*1000/(float)CLOCKS_PER_SEC;
  /// Termino con el log de los resultados:
  strcpy(c_string, "_______RESULTADOS DE RECONSTRUCCION_______");
  logger->writeLine(c_string, (int)strlen(c_string));
  sprintf(c_string, "%f", tiempoTotal);
  logger->writeValue("Tiempo Total de Reconstrucción:", c_string);
  /// Ahora guardo los tiempos por iteración y por etapa, en fila de valores.
  strcpy(c_string, "Tiempos de Reconstrucción por Iteración [mseg]");
  logger->writeLine(c_string, (int)strlen(c_string));
  logger->writeRowOfNumbers(timesIteration_mseg, this->numIterations);
  strcpy(c_string, "Tiempos de Forwardprojection por Iteración [mseg]");
  logger->writeLine(c_string, (int)strlen(c_string));
  logger->writeRowOfNumbers(timesForwardprojection_mseg, this->numIterations);
  strcpy(c_string, "Tiempos de Backwardprojection por Iteración [mseg]");
  logger->writeLine(c_string, (int)strlen(c_string));
  logger->writeRowOfNumbers(timesBackprojection_mseg, this->numIterations);
  strcpy(c_string, "Tiempos de UpdatePixel por Iteración [mseg]");
  logger->writeLine(c_string, (int)strlen(c_string));
  logger->writeRowOfNumbers(timesPixelUpdate_mseg, this->numIterations);
  /// Por último registro los valores de likelihood:
  strcpy(c_string, "Likelihood por Iteración:");
  logger->writeLine(c_string, (int)strlen(c_string));
  logger->writeRowOfNumbers(this->likelihoodValues, this->numIterations + 1);

  /// Libero la memoria de los arrays:
  free(timesIteration_mseg);
  free(timesBackprojection_mseg);
  free(timesForwardprojection_mseg);
  free(timesPixelUpdate_mseg);
  return true;
}

bool Mlem2d::computeSensitivity(Image* outputImage)
{
  /// Creo un Sinograma ·D igual que el de entrada.
  Sinogram2D* backprojectSinogram2D; 
  /// Si no hay normalización lo lleno con un valor constante, de lo contrario bakcprojec normalizacion:
  if (enableMultiplicativeTerm)
    backprojectSinogram2D = multiplicativeProjection;
  else
  {
    backprojectSinogram2D = inputProjection->Copy();
    backprojectSinogram2D->FillConstant(1);
  }
  /// Por último hago la backprojection
  backprojector->Backproject(backprojectSinogram2D, outputImage);
  updateUpdateThreshold();
  return true;
}