#include "PsmTool.h"
#include <vtkProperty.h>


PsmTool::PsmTool(PsmTool::ToolType type = PsmTool::ToolType::None) :
	mtype (type)
{
}

void PsmTool::init(std::string mod_dir)
{
	std::string prefix;
	switch (mtype)
	{
    case PsmTool::ToolType::LND:
		prefix = "lnd";
		mod_dir = mod_dir + "LND/";
		break;
	case PsmTool::ToolType::MCS:
		prefix = "mcs";
		mod_dir = mod_dir + "MCS/";
		break;
	case PsmTool::ToolType::ProGrasp:
		prefix = "pgrasp";
		mod_dir = mod_dir + "PGrasp/";
		break;
	case PsmTool::ToolType::CF:
		prefix = "cf";
		mod_dir = mod_dir + "CF/";
		break;
	case PsmTool::ToolType::MBF:
		prefix = "mbf";
		mod_dir = mod_dir + "MBF/";
		break;
	case PsmTool::ToolType::RTS:
		prefix = "rts";
		mod_dir = mod_dir + "RTS/";
		break;
	default:
		break;
    }

    std::cout << "Loading PSM from " << mod_dir << std::endl;

	/************************************************************************/
	/* Load model and setup VTK pipeline                                    */
	/************************************************************************/

	std::string shaft_ply_name = "shaft_color.ply";
	std::string logo_ply_name = "logobody_color.ply";
	std::string jawL_ply_name = "jawLeft_color.ply";
	std::string jawR_ply_name = "jawRight_color.ply";

	shaft_ply_reader = vtkSmartPointer<vtkPLYReader>::New();
	shaft_ply_reader->SetFileName((mod_dir+shaft_ply_name).c_str());
	shaft_ply_reader->Update();
	shaft_ply_reader->ReleaseDataFlagOn();

	shaft_tris = vtkSmartPointer<vtkTriangleFilter>::New();
	shaft_tris->SetInputConnection(shaft_ply_reader->GetOutputPort());
	shaft_strip = vtkSmartPointer<vtkStripper>::New();
	shaft_strip->SetInputConnection(shaft_tris->GetOutputPort());

	// Logo body
	logo_ply_reader = vtkSmartPointer<vtkPLYReader>::New();
	logo_ply_reader->SetFileName((mod_dir+logo_ply_name).c_str());
	logo_ply_reader->Update();
	logo_ply_reader->ReleaseDataFlagOn();

	logo_tris = vtkSmartPointer<vtkTriangleFilter>::New();
	logo_tris->SetInputConnection(logo_ply_reader->GetOutputPort());
	logo_strip = vtkSmartPointer<vtkStripper>::New();
	logo_strip->SetInputConnection(logo_tris->GetOutputPort());

	// Jaw Left
	jawL_ply_reader = vtkSmartPointer<vtkPLYReader>::New();
	jawL_ply_reader->SetFileName((mod_dir+jawL_ply_name).c_str());
	jawL_ply_reader->Update();
	jawL_ply_reader->ReleaseDataFlagOn();

	jawL_tris = vtkSmartPointer<vtkTriangleFilter>::New();
	jawL_tris->SetInputConnection(jawL_ply_reader->GetOutputPort());
	jawL_strip = vtkSmartPointer<vtkStripper>::New();
	jawL_strip->SetInputConnection(jawL_tris->GetOutputPort());

	// Jaw Right
	jawR_ply_reader = vtkSmartPointer<vtkPLYReader>::New();
	jawR_ply_reader->SetFileName((mod_dir+jawR_ply_name).c_str());
	jawR_ply_reader->Update();
	jawR_ply_reader->ReleaseDataFlagOn();

	jawR_tris = vtkSmartPointer<vtkTriangleFilter>::New();
	jawR_tris->SetInputConnection(jawR_ply_reader->GetOutputPort());
	jawR_strip = vtkSmartPointer<vtkStripper>::New();
	jawR_strip->SetInputConnection(jawR_tris->GetOutputPort());

	// Transform Filter
    base_transform = vtkSmartPointer<vtkTransform>::New();
    base_transform->Identity();

	shaft_transform = vtkSmartPointer<vtkTransform>::New();
	logo_transform = vtkSmartPointer<vtkTransform>::New();
	jawL_transform = vtkSmartPointer<vtkTransform>::New();
	jawR_transform = vtkSmartPointer<vtkTransform>::New();
	
	// M = A * M (A is the applied transform)
	base_transform->PreMultiply();
	shaft_transform->PreMultiply();
	logo_transform->PreMultiply();
	jawL_transform->PreMultiply();
	jawR_transform->PreMultiply();

	shaft_transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	logo_transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	jawL_transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	jawR_transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

	shaft_transform_filter->SetInputConnection(shaft_strip->GetOutputPort());
	logo_transform_filter->SetInputConnection(logo_strip->GetOutputPort());
	jawL_transform_filter->SetInputConnection(jawL_strip->GetOutputPort());
	jawR_transform_filter->SetInputConnection(jawR_strip->GetOutputPort());

	shaft_transform_filter->SetTransform(shaft_transform);
	logo_transform_filter->SetTransform(logo_transform);
	jawL_transform_filter->SetTransform(jawL_transform);
	jawR_transform_filter->SetTransform(jawR_transform);

	shaft_transform_filter->Update();
	logo_transform_filter->Update();
	jawL_transform_filter->Update();
	jawR_transform_filter->Update();

	// Mapper
	shaft_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	logo_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	jawL_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	jawR_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    // May speed up for large number of polygons

	shaft_mapper->SetInputConnection(shaft_transform_filter->GetOutputPort());
	logo_mapper->SetInputConnection(logo_transform_filter->GetOutputPort());
	jawL_mapper->SetInputConnection(jawL_transform_filter->GetOutputPort());
	jawR_mapper->SetInputConnection(jawR_transform_filter->GetOutputPort());

	// Actor
	shaft_actor = vtkSmartPointer<vtkActor>::New();
	logo_actor = vtkSmartPointer<vtkActor>::New();
	jawL_actor = vtkSmartPointer<vtkActor>::New();
	jawR_actor = vtkSmartPointer<vtkActor>::New();

	shaft_actor->SetMapper(shaft_mapper);
	logo_actor->SetMapper(logo_mapper);
	jawL_actor->SetMapper(jawL_mapper);
	jawR_actor->SetMapper(jawR_mapper);

    shaft_actor->GetProperty()->SetInterpolationToFlat();
    logo_actor->GetProperty()->SetInterpolationToFlat();
    jawL_actor->GetProperty()->SetInterpolationToFlat();
    jawR_actor->GetProperty()->SetInterpolationToFlat();

	// Axis
	shaft_axes = vtkSmartPointer<vtkAxesActor>::New();
    logo_axes  = vtkSmartPointer<vtkAxesActor>::New();
    jaw_axes   = vtkSmartPointer<vtkAxesActor>::New();
	shaft_axes->SetTotalLength(15, 15, 15);
	logo_axes->SetTotalLength(10, 10, 10);
	jaw_axes->SetTotalLength(20, 20, 20);

}

void PsmTool::Update_Base_Transform (const double * elements) {
    base_transform->SetMatrix(elements);
    base_transform->Update();
}

void PsmTool::Update_Shaft_Transform (const double *elements)
{
    shaft_transform->Identity();
    shaft_transform->Concatenate(base_transform);
    shaft_transform->Concatenate(elements);
	shaft_transform->Update();
}

void PsmTool::Update_Logo_Transform (const double *elements)
{
	logo_transform->Identity();
    logo_transform->Concatenate(base_transform);
    logo_transform->Concatenate(elements);
	logo_transform->Update();
}

void PsmTool::Update_JawL_Transform (const double *elements)
{
	jawL_transform->Identity();
    jawL_transform->Concatenate(base_transform);
    jawL_transform->Concatenate(elements);
	jawL_transform->Update();
}

void PsmTool::Update_JawR_Transform (const double *elements)
{
	jawR_transform->Identity();
    jawR_transform->Concatenate(base_transform);
    jawR_transform->Concatenate(elements);
	jawR_transform->Update();
}
