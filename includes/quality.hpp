#pragma once

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkMeshQuality.h>

class QualityChecker
{
  vtkSmartPointer<vtkUnstructuredGrid> _data;
  vtkSmartPointer<vtkUnstructuredGrid> _output;

  QualityChecker() {}

public:
  ~QualityChecker(void) {}

  QualityChecker(vtkSmartPointer<vtkUnstructuredGrid> data): _data(data), _output(vtkSmartPointer<vtkUnstructuredGrid>::New()){}

  virtual void Update(void){
    vtkSmartPointer<vtkMeshQuality> qualityFilter = vtkSmartPointer<vtkMeshQuality>::New();
    qualityFilter->SetInputData(_data);
    qualityFilter->SetTriangleQualityMeasureToShapeAndSize();
    qualityFilter->Update();

    _output->ShallowCopy(qualityFilter->GetOutput());
  }

  virtual vtkSmartPointer<vtkUnstructuredGrid> GetOutput(void){
    return _output;
  }

};
