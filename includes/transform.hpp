#pragma once

#include <vtkUnstructuredGridReader.h>
#include <vtkSmartPointer.h>
#include <vtkGeneralTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkXMLWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkXMLWriter.h>
#include <vtkTransformFilter.h>
#include <vtkAppendFilter.h>

#include "quality.hpp"

class Transformation
{

protected:
  std::string _outputFile;
  bool _computeQuality;

  Transformation(void){}

  const void write(vtkSmartPointer<vtkUnstructuredGrid> data){
    vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
    writer->SetFileName(_outputFile.c_str());
    writer->SetInputData(data);
    writer->Write();
  }

  virtual vtkSmartPointer<vtkUnstructuredGrid> transform(void) = 0;

public:
  virtual ~Transformation(void) = default;

  Transformation( const std::string outputFile,
                  const bool computeQuality):
                  _outputFile(outputFile),
                  _computeQuality(computeQuality){}

  virtual void Update(void){

    vtkSmartPointer<vtkUnstructuredGrid> data = transform();

    if(_computeQuality){

      QualityChecker* qc = new QualityChecker(data);
      qc->Update();
      write(qc->GetOutput());
      delete qc;

    }else{
      write(data);
    }
  }

  const std::string GetOutput(void){
    return _outputFile;
  }
};


class Merge : public Transformation
{
  bool _mergeNodes;
  std::vector<std::string> _inputFiles;

  Merge(void) {}
  ~Merge(void) {}

public:
  Merge( const bool mergeNodes,
         const std::vector<std::string> meshes,
         const std::string resultFileName,
         const bool computeQuality):
         Transformation(resultFileName, computeQuality),
         _mergeNodes(mergeNodes),
         _inputFiles(meshes) {}

  virtual vtkSmartPointer<vtkUnstructuredGrid> transform(void){
    auto readers = new vtkSmartPointer<vtkXMLUnstructuredGridReader>[_inputFiles.size()];

    vtkSmartPointer<vtkAppendFilter> af = vtkSmartPointer<vtkAppendFilter>::New();
    af->SetMergePoints(_mergeNodes);
    //af->SetTolerance(1e-16);

    int i = 0;
    for(auto& file : _inputFiles){
      readers[i] = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
      readers[i]->SetFileName(file.c_str());
      readers[i]->Update();
      af->AddInputConnection(readers[i]->GetOutputPort());
      af->Update();
      i++;
    }

    delete[] readers;
    return af->GetOutput();
  }
};

class Translation : public Transformation
{
  std::string _inputFile;
  std::vector<double> _vector;

  Translation(void) {}
  ~Translation(void) {}

public:
  Translation(const std::vector<double> coords,
              const std::string mesh,
              const std::string resultFileName,
              const bool computeQuality):
              Transformation(resultFileName, computeQuality),
              _vector(coords),
              _inputFile(mesh) {}

  virtual vtkSmartPointer<vtkUnstructuredGrid> transform(void){
    vtkSmartPointer<vtkXMLUnstructuredGridReader> reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    reader->SetFileName(_inputFile.c_str());
    reader->Update();

    vtkSmartPointer<vtkGeneralTransform> tr = vtkSmartPointer<vtkGeneralTransform>::New();
    tr->Translate(_vector[0], _vector[1], _vector[2]);

    vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    transformFilter->SetInputConnection(reader->GetOutputPort());
    transformFilter->SetTransform(tr);
    transformFilter->Update();

    return vtkUnstructuredGrid::SafeDownCast(transformFilter->GetOutput());
  }
};
