#pragma once

// c++ std library
#include <string>

// VTK
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkAxesActor.h>


// OpenCV 2
#include <opencv2/core/core.hpp>
#include <unordered_map>


class PsmTool
{
public:
	enum class ToolType
	{
		LND,
		MCS,
		ProGrasp,
		CF,
		MBF,
		RTS,
		None
	};
	PsmTool (ToolType type);

    void init(std::string mod_dir);

	void Update_Base_Transform (const double *elements);

	// Update Transform
	void Update_Shaft_Transform (const double *elements);

	void Update_Logo_Transform (const double *elements);

	void Update_JawL_Transform (const double *elements);

	void Update_JawR_Transform (const double *elements);

	// Get VTK Related
	vtkSmartPointer<vtkActor> Shaft_Actor () { return shaft_actor; }
	vtkSmartPointer<vtkActor> Logo_Actor () { return logo_actor; }
	vtkSmartPointer<vtkActor> JawL_Actor () { return jawL_actor; }
	vtkSmartPointer<vtkActor> JawR_Actor () { return jawR_actor; }


	std::vector<std::string> partnames = {
			("shaft_pivot_flat"),
			("shaft_pivot_deep"),
			("logo_pin_flat"),
			("logo_pin_deep"),
			("logo_wheel_flat"),
			("logo_wheel_deep"),
			("logo_is_flat"),
			("logo_is_deep"),
			("logo_idot_flat"),
			("logo_idot_deep"),
			("logo_pivot_flat"),
			("logo_pivot_deep"),
			("tip_flat"),
			("tip_deep"),
			("shaft_centre")
	};

	std::vector<cv::Scalar> partcolors = {
			(cv::Scalar(0,255,255)),
			(cv::Scalar(0,255,255)),   // yellow shaft_pivot flat & deep
			(cv::Scalar(0,153,255)),
			(cv::Scalar(0,153,255)),   // orange logo_pin flat & deep
			(cv::Scalar(0,0,255)),
			(cv::Scalar(0,0,255)),     // red logo_wheel flat & deep
			(cv::Scalar(255,255,0)),
			(cv::Scalar(255,255,0)),   // cyan logo_is flat & deep
			(cv::Scalar(190,190,190)),
			(cv::Scalar(190,190,190)), // gray logo_idot flat & deep
			(cv::Scalar(255,0,0)),
			(cv::Scalar(255,0,0)),     // blue logo_pivot flat & deep
			(cv::Scalar(255,0,255)),   // magenta tip_flat
			(cv::Scalar(0,255,0)),     //green tip_deep
			(cv::Scalar(255,255,150)), //light blue
	};;

	std::unordered_map<std::string, unsigned int> partname_ids = {
			(std::make_pair("shaft_pivot_flat", 0)),
			(std::make_pair("shaft_pivot_deep", 1)),
			(std::make_pair("logo_pin_flat", 2)),
			(std::make_pair("logo_pin_deep", 3)),
			(std::make_pair("logo_wheel_flat", 4)),
			(std::make_pair("logo_wheel_deep", 5)),
			(std::make_pair("logo_is_flat", 6)),
			(std::make_pair("logo_is_deep", 7)),
			(std::make_pair("logo_idot_flat", 8)),
			(std::make_pair("logo_idot_deep", 9)),
			(std::make_pair("logo_pivot_flat", 10)),
			(std::make_pair("logo_pivot_deep", 11)),
			(std::make_pair("tip_flat", 12)),
			(std::make_pair("tip_deep", 13)),
			(std::make_pair("shaft_centre", 14)),
	};

	std::vector<float> part_torlerances = {
    		1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			3.0f,
			3.0f,
			1.0f,
	};

protected:

private:
	ToolType mtype;

	// VTK stl reader
	vtkSmartPointer<vtkSTLReader> shaft_stl_reader;
	vtkSmartPointer<vtkSTLReader> logo_stl_reader;
	vtkSmartPointer<vtkSTLReader> jawL_stl_reader;
	vtkSmartPointer<vtkSTLReader> jawR_stl_reader;

	// VTK ply reader
	vtkSmartPointer<vtkPLYReader> shaft_ply_reader;
	vtkSmartPointer<vtkPLYReader> logo_ply_reader;
	vtkSmartPointer<vtkPLYReader> jawL_ply_reader;
	vtkSmartPointer<vtkPLYReader> jawR_ply_reader;

	// VTK mapper
	vtkSmartPointer<vtkPolyDataMapper> shaft_mapper;
	vtkSmartPointer<vtkPolyDataMapper> logo_mapper;
	vtkSmartPointer<vtkPolyDataMapper> jawL_mapper;
	vtkSmartPointer<vtkPolyDataMapper> jawR_mapper;

	// VTK actor
	vtkSmartPointer<vtkActor> shaft_actor;
	vtkSmartPointer<vtkActor> logo_actor;
	vtkSmartPointer<vtkActor> jawL_actor;
	vtkSmartPointer<vtkActor> jawR_actor;

	// VTK TriangleFilter (for fast rendering)
	vtkSmartPointer<vtkTriangleFilter> shaft_tris;
	vtkSmartPointer<vtkTriangleFilter> logo_tris;
	vtkSmartPointer<vtkTriangleFilter> jawL_tris;
	vtkSmartPointer<vtkTriangleFilter> jawR_tris;

	// VTK vtkStripper (for fast rendering)
	vtkSmartPointer<vtkStripper> shaft_strip;
	vtkSmartPointer<vtkStripper> logo_strip;
	vtkSmartPointer<vtkStripper> jawL_strip;
	vtkSmartPointer<vtkStripper> jawR_strip;

	vtkSmartPointer<vtkTransform> base_transform;
	vtkSmartPointer<vtkTransform> shaft_transform;
	vtkSmartPointer<vtkTransform> logo_transform;
	vtkSmartPointer<vtkTransform> jawL_transform;
	vtkSmartPointer<vtkTransform> jawR_transform;

	vtkSmartPointer<vtkTransformPolyDataFilter> shaft_transform_filter;
	vtkSmartPointer<vtkTransformPolyDataFilter> logo_transform_filter;
	vtkSmartPointer<vtkTransformPolyDataFilter> jaw_transform_filter;
	vtkSmartPointer<vtkTransformPolyDataFilter> jawL_transform_filter;
	vtkSmartPointer<vtkTransformPolyDataFilter> jawR_transform_filter;

	vtkSmartPointer<vtkAxesActor> shaft_axes;
	vtkSmartPointer<vtkAxesActor> logo_axes;
	vtkSmartPointer<vtkAxesActor> jaw_axes;
};





