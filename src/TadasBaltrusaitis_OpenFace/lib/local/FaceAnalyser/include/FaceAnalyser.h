///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016, Carnegie Mellon University and University of Cambridge,
// all rights reserved.
//
// THIS SOFTWARE IS PROVIDED �AS IS?FOR ACADEMIC USE ONLY AND ANY EXPRESS
// OR IMPLIED WARRANTIES WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY.
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Notwithstanding the license granted herein, Licensee acknowledges that certain components
// of the Software may be covered by so-called �open source?software licenses (�Open Source
// Components?, which means any software licenses approved as open source licenses by the
// Open Source Initiative or any substantially similar licenses, including without limitation any
// license that, as a condition of distribution of the software licensed under such license,
// requires that the distributor make the software available in source code format. Licensor shall
// provide a list of Open Source Components for a particular version of the Software upon
// Licensee�s request. Licensee will comply with the applicable terms of such licenses and to
// the extent required by the licenses covering Open Source Components, the terms of such
// licenses will apply in lieu of the terms of this Agreement. To the extent the terms of the
// licenses applicable to Open Source Components prohibit any of the restrictions in this
// License Agreement with respect to such Open Source Component, such restrictions will not
// apply to such Open Source Component. To the extent the terms of the licenses applicable to
// Open Source Components require Licensor to make an offer to provide source code or
// related information in connection with the Software, such offer is hereby made. Any request
// for source code or related information should be directed to cl-face-tracker-distribution@lists.cam.ac.uk
// Licensee acknowledges receipt of notices for the Open Source Components for the initial
// delivery of the Software.

//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite at least one of the following works:
//
//       OpenFace: an open source facial behavior analysis toolkit
//       Tadas Baltru�aitis, Peter Robinson, and Louis-Philippe Morency
//       in IEEE Winter Conference on Applications of Computer Vision, 2016  
//
//       Rendering of Eyes for Eye-Shape Registration and Gaze Estimation
//       Erroll Wood, Tadas Baltru�aitis, Xucong Zhang, Yusuke Sugano, Peter Robinson, and Andreas Bulling 
//       in IEEE International. Conference on Computer Vision (ICCV),  2015 
//
//       Cross-dataset learning and person-speci?c normalisation for automatic Action Unit detection
//       Tadas Baltru�aitis, Marwa Mahmoud, and Peter Robinson 
//       in Facial Expression Recognition and Analysis Challenge, 
//       IEEE International Conference on Automatic Face and Gesture Recognition, 2015 
//
//       Constrained Local Neural Fields for robust facial landmark detection in the wild.
//       Tadas Baltru�aitis, Peter Robinson, and Louis-Philippe Morency. 
//       in IEEE Int. Conference on Computer Vision Workshops, 300 Faces in-the-Wild Challenge, 2013.    
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FACEANALYSER_h_
#define __FACEANALYSER_h_

#include "SVR_dynamic_lin_regressors.h"
#include "SVR_static_lin_regressors.h"
#include "SVM_static_lin.h"
#include "SVM_dynamic_lin.h"

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

#include "LandmarkCoreIncludes.h"

namespace FaceAnalysis
{

class FaceAnalyser{

public:


	enum RegressorType{ SVR_appearance_static_linear = 0, SVR_appearance_dynamic_linear = 1, SVR_dynamic_geom_linear = 2, SVR_combined_linear = 3, SVM_linear_stat = 4, SVM_linear_dyn = 5, SVR_linear_static_seg = 6, SVR_linear_dynamic_seg =7};

	// Constructor from a model file (or a default one if not provided
	// TODO scale width and height should be read in as part of the model as opposed to being here?
	FaceAnalyser(vector<cv::Vec3d> orientation_bins = vector<cv::Vec3d>(), double scale = 0.7, int width = 112, int height = 112, std::string au_location = "AU_predictors/AU_all_best.txt", std::string tri_location = "model/tris_68_full.txt");

	void AddNextFrame(const cv::Mat& frame, const LandmarkDetector::CLNF& clnf, double timestamp_seconds, bool online = false, bool visualise = true);

	// If the features are extracted manually (shouldn't really be used)
	void PredictAUs(const cv::Mat_<double>& hog_features, const cv::Mat_<double>& geom_features, const LandmarkDetector::CLNF& clnf_model, bool online);

	cv::Mat GetLatestHOGDescriptorVisualisation();

	double GetCurrentTimeSeconds();
	
	// Grab the current predictions about AUs from the face analyser
	std::vector<std::pair<std::string, double>> GetCurrentAUsClass() const; // AU presence
	std::vector<std::pair<std::string, double>> GetCurrentAUsReg() const;   // AU intensity
	std::vector<std::pair<std::string, double>> GetCurrentAUsCombined() const; // Both presense and intensity

	// A standalone call for predicting AUs from a static image, the first element in the pair represents occurence the second intensity
	// This call is useful for detecting action units in images
	std::pair<std::vector<std::pair<string, double>>, std::vector<std::pair<string, double>>> PredictStaticAUs(const cv::Mat& frame, const LandmarkDetector::CLNF& clnf, bool visualise = true);

	void Reset();

	void GetLatestHOG(cv::Mat_<double>& hog_descriptor, int& num_rows, int& num_cols);
	void GetLatestAlignedFace(cv::Mat& image);
	
	void GetLatestNeutralHOG(cv::Mat_<double>& hog_descriptor, int& num_rows, int& num_cols);
	
	cv::Mat_<int> GetTriangulation();

	cv::Mat_<uchar> GetLatestAlignedFaceGrayscale();
	
	void GetGeomDescriptor(cv::Mat_<double>& geom_desc);

	void ExtractCurrentMedians(vector<cv::Mat>& hog_medians, vector<cv::Mat>& face_image_medians, vector<cv::Vec3d>& orientations);

	// Grab the names of AUs being predicted
	std::vector<std::string> GetAUClassNames() const; // Presence
	std::vector<std::string> GetAURegNames() const; // Intensity

	// Identify if models are static or dynamic (useful for correction and shifting)
	std::vector<bool> GetDynamicAUClass() const; // Presence
	std::vector<std::pair<string, bool>> GetDynamicAUReg() const; // Intensity


	void ExtractAllPredictionsOfflineReg(vector<std::pair<std::string, vector<double>>>& au_predictions, vector<double>& confidences, vector<bool>& successes, vector<double>& timestamps, bool dynamic);
	void ExtractAllPredictionsOfflineClass(vector<std::pair<std::string, vector<double>>>& au_predictions, vector<double>& confidences, vector<bool>& successes, vector<double>& timestamps, bool dynamic);

private:

	// Where the predictions are kept
	std::vector<std::pair<std::string, double>> AU_predictions_reg;
	std::vector<std::pair<std::string, double>> AU_predictions_class;

	std::vector<std::pair<std::string, double>> AU_predictions_combined;

	// Keeping track of AU predictions over time (useful for post-processing)
	vector<double> timestamps;
	std::map<std::string, vector<double>> AU_predictions_reg_all_hist;
	std::map<std::string, vector<double>> AU_predictions_class_all_hist;
	std::vector<double> confidences;
	std::vector<bool> valid_preds;

	int frames_tracking;

	// Cache of intermediate images
	cv::Mat_<uchar> aligned_face_grayscale;
	cv::Mat aligned_face;
	cv::Mat hog_descriptor_visualisation;

	// Private members to be used for predictions
	// The HOG descriptor of the last frame
	cv::Mat_<double> hog_desc_frame;
	int num_hog_rows;
	int num_hog_cols;

	// Keep a running median of the hog descriptors and a aligned images
	cv::Mat_<double> hog_desc_median;
	cv::Mat_<double> face_image_median;

	// Use histograms for quick (but approximate) median computation
	// Use the same for
	vector<cv::Mat_<float> > hog_desc_hist;

	// This is not being used at the moment as it is a bit slow
	vector<cv::Mat_<float> > face_image_hist;
	vector<int> face_image_hist_sum;

	vector<cv::Vec3d> head_orientations;

	int num_bins_hog;
	double min_val_hog;
	double max_val_hog;
	vector<int> hog_hist_sum;
	int view_used;

	// The geometry descriptor (rigid followed by non-rigid shape parameters from CLNF)
	cv::Mat_<double> geom_descriptor_frame;
	cv::Mat_<double> geom_descriptor_median;
	
	int geom_hist_sum;
	cv::Mat_<float> geom_desc_hist;
	int num_bins_geom;
	double min_val_geom;
	double max_val_geom;
	
	// Using the bounding box of previous analysed frame to determine if a reset is needed
	cv::Rect_<double> face_bounding_box;
	
	// The AU predictions internally
	std::vector<std::pair<std::string, double>> PredictCurrentAUs(int view);
	std::vector<std::pair<std::string, double>> PredictCurrentAUsClass(int view);

	// special step for online (rather than offline AU prediction)
	std::vector<pair<string, double>> CorrectOnlineAUs(std::vector<std::pair<std::string, double>> predictions_orig, int view, bool dyn_shift = false, bool dyn_scale = false, bool update_track = true, bool clip_values = false);

	void ReadAU(std::string au_location);

	void ReadRegressor(std::string fname, const vector<string>& au_names);

	// A utility function for keeping track of approximate running medians used for AU and emotion inference using a set of histograms (the histograms are evenly spaced from min_val to max_val)
	// Descriptor has to be a row vector
	// TODO this duplicates some other code
	void UpdateRunningMedian(cv::Mat_<float>& histogram, int& hist_sum, cv::Mat_<double>& median, const cv::Mat_<double>& descriptor, bool update, int num_bins, double min_val, double max_val);
	void ExtractMedian(cv::Mat_<float>& histogram, int hist_count, cv::Mat_<double>& median, int num_bins, double min_val, double max_val);
	
	// The linear SVR regressors
	SVR_static_lin_regressors AU_SVR_static_appearance_lin_regressors;
	SVR_dynamic_lin_regressors AU_SVR_dynamic_appearance_lin_regressors;
		
	// The linear SVM classifiers
	SVM_static_lin AU_SVM_static_appearance_lin;
	SVM_dynamic_lin AU_SVM_dynamic_appearance_lin;

	// The AUs predicted by the model are not always 0 calibrated to a person. That is they don't always predict 0 for a neutral expression
	// Keeping track of the predictions we can correct for this, by assuming that at least "ratio" of frames are neutral and subtract that value of prediction, only perform the correction after min_frames
	void UpdatePredictionTrack(cv::Mat_<float>& prediction_corr_histogram, int& prediction_correction_count, vector<double>& correction, const vector<pair<string, double>>& predictions, double ratio=0.25, int num_bins = 200, double min_val = -3, double max_val = 5, int min_frames = 10);
	void GetSampleHist(cv::Mat_<float>& prediction_corr_histogram, int prediction_correction_count, vector<double>& sample, double ratio, int num_bins = 200, double min_val = 0, double max_val = 5);

	void PostprocessPredictions();

	vector<cv::Mat_<float>> au_prediction_correction_histogram;
	vector<int> au_prediction_correction_count;

	// Some dynamic scaling (the logic is that before the extreme versions of expression or emotion are shown,
	// it is hard to tell the boundaries, this allows us to scale the model to the most extreme seen)
	// They have to be view specific
	vector<vector<double>> dyn_scaling;
	
	// Keeping track of predictions for summary stats
	cv::Mat_<double> AU_prediction_track;
	cv::Mat_<double> geom_desc_track;

	double current_time_seconds;

	// Used for face alignment
	cv::Mat_<int> triangulation;
	double align_scale;	
	int align_width;
	int align_height;

	// Useful placeholder for renormalizing the initial frames of shorter videos
	int max_init_frames = 3000;
	vector<cv::Mat_<double>> hog_desc_frames_init;
	vector<cv::Mat_<double>> geom_descriptor_frames_init;
	vector<int> views;
	bool postprocessed = false;
	int frames_tracking_succ = 0;

};
  //===========================================================================
}
#endif
