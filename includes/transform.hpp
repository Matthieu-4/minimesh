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

class Transformation
{

protected:
  std::string _outputFile;
  bool _computeQuality;

  Transformation(void){}

  const void write(vtkDataSet* data){
    vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
    writer->SetFileName(_outputFile.c_str());
    writer->SetInputData(data);
    writer->Write();
  }
public:
  virtual ~Transformation(void) = default;
  Transformation( const std::string outputFile,
                  const bool computeQuality):
                  _outputFile(outputFile),
                  _computeQuality(computeQuality){}

  virtual void transform(void) = 0;

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

  virtual void transform(void){
    /* TODO */
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

  virtual void transform(void){
    vtkSmartPointer<vtkXMLUnstructuredGridReader> reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();

    reader->SetFileName(_inputFile.c_str());
    reader->Update();
    vtkSmartPointer<vtkGeneralTransform> tr = vtkSmartPointer<vtkGeneralTransform>::New();
    tr->Translate(_vector[0], _vector[1], _vector[2]);
    vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    transformFilter->SetInputConnection(reader->GetOutputPort());
    transformFilter->SetTransform(tr);
    transformFilter->Update();
    write(transformFilter->GetOutput());
  }
};
