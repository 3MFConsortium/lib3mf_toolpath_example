/*++

Copyright (C) 2025 3MF Consortium

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Autodesk Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <iostream>
#include <vector>
#include "lib3mf_dynamic.hpp"

// Convenience function to create a box geometry
void createBoxMesh(Lib3MF::PMeshObject pMeshObject, float sizex, float sizey, float sizez)
{
    std::vector<Lib3MF::sPosition> Vertices;
    std::vector<Lib3MF::sTriangle> Triangles;

    Vertices.push_back({ 0.0, 0.0, 0.0 });
    Vertices.push_back({ sizex, 0.0, 0.0 });
    Vertices.push_back({ sizex, sizey, 0.0 });
    Vertices.push_back({ 0.0, sizey, 0.0 });
    Vertices.push_back({ 0.0, 0.0, sizez });
    Vertices.push_back({ sizex, 0.0, sizez });
    Vertices.push_back({ sizex, sizey, sizez });
    Vertices.push_back({ 0.0, sizey, sizez });

    Triangles.push_back({ 2, 1, 0 });
    Triangles.push_back({ 0, 3, 2 });
    Triangles.push_back({ 4, 5, 6 });
    Triangles.push_back({ 6, 7, 4 });
    Triangles.push_back({ 0, 1, 5 });
    Triangles.push_back({ 5, 4, 0 });
    Triangles.push_back({ 1, 2, 6 });
    Triangles.push_back({ 6, 5, 1 });
    Triangles.push_back({ 2, 3, 7 });
    Triangles.push_back({ 7, 6, 2 });
    Triangles.push_back({ 3, 0, 4 });
    Triangles.push_back({ 4, 7, 3 });

    pMeshObject->SetGeometry(Vertices, Triangles);
}


// Demo that writes a toolpath file
void writeToolpathDemo (Lib3MF::PWrapper p3MFWrapper, const std::string sOutputFileName)
{
    auto pModel = p3MFWrapper->CreateModel();

    // add Toolpath entity. Units are micron.
    auto pToolpath = pModel->AddToolpathWithBottomZ(0.001, 0);

    // Not needed, but just for info how metadata is added...
    auto pCustomData = pToolpath->AddCustomData("http://mycompany.com/mycustomdata", "testmetadata");
    auto pCustomDataNode = pCustomData->GetRootNode();
    auto pToolpathInfoNode = pCustomDataNode->AddChild("toolpathinfo");
    pToolpathInfoNode->AddAttribute("used_compensation_mode", "simple");
    pToolpathInfoNode->AddAttribute("machine_model", "SLM500");


    // There is also a way to add custom files as attachment
    auto pAttachment = pModel->AddAttachment("/mycompanydata/attachment.json", "http://mycompany.com/customattachment");

    std::string sAttachmentData = "{ \"test\": 1234 }";
    std::vector<uint8_t> attachmentBuffer (sAttachmentData.begin (), sAttachmentData.end ());
    pAttachment->ReadFromBuffer(attachmentBuffer);


    // Add Contour Profile
    auto pContourProfile = pToolpath->AddProfile("contour_profile");

    // Set generic laser parameters (no namespace)
    pContourProfile->SetParameterDoubleValue("", "laserpower", 125.0);
    pContourProfile->SetParameterDoubleValue("", "laserspeed", 500.0);
    pContourProfile->SetParameterDoubleValue("", "jumpspeed", 1000.0);
    pContourProfile->SetParameterDoubleValue("", "laserfocus", 1.0);
    // Set machine specific laser parameters (custom namespace)
    pContourProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "mode", 3);
    pContourProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "timelag", 550.0);
    pContourProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "laseronshift", 0);
    pContourProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "limit", 0);
    pContourProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "nprev", 170);
    pContourProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "npost", 200);
    pContourProfile->AddModifier("", "laserpower", Lib3MF::eToolpathProfileModificationType::NonlinearModification, Lib3MF::eToolpathProfileModificationFactor::FactorF, 0.0, +300.0);
    pContourProfile->AddModifier("", "laserspeed", Lib3MF::eToolpathProfileModificationType::ConstantModification, Lib3MF::eToolpathProfileModificationFactor::FactorG, 0.0, 100.0);

    // Add Additional Profile
    auto pAdditionalProfile = pToolpath->AddProfile("additional_profile");

    // Set generic laser parameters (no namespace)
    pAdditionalProfile->SetParameterDoubleValue("", "laserpower", 250.0);
    pAdditionalProfile->SetParameterDoubleValue("", "laserspeed", 240.0);
    pAdditionalProfile->SetParameterDoubleValue("", "jumpspeed", 120.0);
    pAdditionalProfile->SetParameterDoubleValue("", "laserfocus", 0.0);

    // Set machine specific laser parameters (custom namespace)
    pAdditionalProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "mode", 3);
    pAdditionalProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "timelag", 550.0);
    pAdditionalProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "laseronshift", 0);
    pAdditionalProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "limit", 0);
    pAdditionalProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "nprev", 170);
    pAdditionalProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "npost", 200);


    // Add 3D Geometry (optional, can be outbox)
    auto pMeshObject = pModel->AddMeshObject ();
    createBoxMesh(pMeshObject, 20, 30, 40);

    // Add Instances of Mesh Object to scene (could be multiple)
    auto pBuildItem = pModel->AddBuildItem(pMeshObject.get(), p3MFWrapper->GetIdentityTransform());

    // Add Hatch Profile
    auto pHatchProfile = pToolpath->AddProfile("hatch_profile");

    // Set generic laser parameters (no namespace)
    pHatchProfile->SetParameterDoubleValue("", "laserpower", 400.0);
    pHatchProfile->SetParameterDoubleValue("", "laserspeed", 600.0);
    pHatchProfile->SetParameterDoubleValue("", "jumpspeed", 1000.0);
    pHatchProfile->SetParameterDoubleValue("", "laserfocus", 0.0);
    pHatchProfile->AddModifier("", "laserpower", Lib3MF::eToolpathProfileModificationType::NonlinearModification, Lib3MF::eToolpathProfileModificationFactor::FactorF, 0.0, +300.0);
    pHatchProfile->AddModifier("", "laserspeed", Lib3MF::eToolpathProfileModificationType::ConstantModification, Lib3MF::eToolpathProfileModificationFactor::FactorG, 0.0, 100.0);

    // Set machine specific laser parameters (custom namespace)
    pHatchProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "mode", 3);
    pHatchProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "timelag", 550.0);
    pHatchProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "laseronshift", 0);
    pHatchProfile->SetParameterDoubleValue("http://schemas.scanlab.com/skywriting/2023/01", "limit", 0);
    pHatchProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "nprev", 170);
    pHatchProfile->SetParameterIntegerValue("http://schemas.scanlab.com/skywriting/2023/01", "npost", 200);

    // Create a model writer instance
    auto pWriter = pModel->QueryWriter("3mf");

    // Define namespaces to be written out
    pWriter->RegisterCustomNamespace("mycompany", "http://mycompany.com/mycustomdata");
    pWriter->RegisterCustomNamespace("skywriting", "http://schemas.scanlab.com/skywriting/2023/01");
    pWriter->SetCustomNamespaceRequired("mycompany", true);

    // Write Layers
    for (uint32_t nLayerIndex = 1; nLayerIndex <= 5; nLayerIndex++) {

        std::cout << "Writing layer " << nLayerIndex << std::endl;

        // Write with 0.05mm layer thickness
        uint32_t nZHeightInMicron = nLayerIndex * 50;

        // Path in 3MF Package should be something like this
        std::string sLayerPath = "/Toolpath/layer" + std::to_string(nLayerIndex) + ".xml";
        std::string sLayerIndexPath = "/Toolpath/layer" + std::to_string(nLayerIndex) + "_data.xml";
        std::string sLayerBinaryPath = "/Toolpath/layer" + std::to_string(nLayerIndex) + "_data.bin";

        //auto pBinaryStream = pWriter->CreateBinaryStream(sLayerIndexPath, sLayerBinaryPath);
        //pBinaryStream->EnableLZ4(12);
           
        // Create Layer Object
        auto pLayer = pToolpath->AddLayer(nZHeightInMicron, sLayerPath, pWriter);

        //pWriter->AssignBinaryStream(pLayer.get(), pBinaryStream);

        // Add MetaData to Layer
        auto pCustomData = pLayer->AddCustomData("http://mycompany.com/mycustomdata", "layermetadata");
        auto pCustomDataNode = pCustomData->GetRootNode();
        auto pToolpathInfoNode = pCustomDataNode->AddChild("layerinfo");
        pToolpathInfoNode->AddAttribute("recoating_speed", "1234.0");

        // Register profiles to be used.
        uint32_t nContourProfileID = pLayer->RegisterProfile(pContourProfile);
        uint32_t nHatchProfileID = pLayer->RegisterProfile(pHatchProfile);
        uint32_t nAdditionalProfileID = pLayer->RegisterProfile(pAdditionalProfile);
        uint32_t nPartID = pLayer->RegisterBuildItem(pBuildItem);

        // Write a dummy contour
        std::vector<Lib3MF::sPosition2D> contourPoints;
        std::vector<double> scaleFactorsF;
        std::vector<uint32_t> subInterpolationCounts;
        std::vector<Lib3MF::sHatchModificationInterpolationData> subInterpolationData;

        contourPoints.push_back({ 0.0f, 0.0f });
        scaleFactorsF.push_back(0.32);
        
        contourPoints.push_back({ 20.0f, 0.0f });
        scaleFactorsF.push_back(0.425);

        contourPoints.push_back({ 20.0f, 30.0f });
        scaleFactorsF.push_back(0.525);

        contourPoints.push_back({ 0.0f, 30.0f });
        scaleFactorsF.push_back(0.617);

        pLayer->WriteLoopInModelUnitsWithFactors(nContourProfileID, nPartID, contourPoints, scaleFactorsF);

        // Write a dummy hatches
        std::vector<Lib3MF::sHatch2D> hatchList;
        std::vector<double> scaleParameterList1;
        std::vector<double> scaleParameterList2;
        for (uint32_t nHatchIndex = 1; nHatchIndex < 1000; nHatchIndex++) {
            float y = nHatchIndex * 0.015f;
            Lib3MF::sHatch2D hatch;
            
            hatch.m_Point1Coordinates[0] = 0.1f;
            hatch.m_Point1Coordinates[1] = y;            
            hatch.m_Point2Coordinates[0] = 19.9f;
            hatch.m_Point2Coordinates[1] = y;

            double f1, f2;
               
            // Set a custom tag on each second hatch
            if (nHatchIndex % 2 == 0) {
                hatch.m_Tag = nHatchIndex * 2 + 5;
                f1 = 0.3;
                f2 = 0.7;
            }
            else {
                f2 = 0.3;
                f1 = 0.7;
                hatch.m_Tag = 0;
            }

            scaleParameterList1.push_back(f1);
            scaleParameterList2.push_back(f2);

            // Add hatch
            hatchList.push_back(hatch);

            uint32_t nCount = 40;

            if (nHatchIndex % 3 == 0)
                nCount = 50;

            if (nHatchIndex == 7)
                nCount = 9;

            if (nHatchIndex == 16)
                nCount = 18;

            for (uint32_t nSubIndex = 1; nSubIndex <= nCount; nSubIndex++) {
                Lib3MF::sHatchModificationInterpolationData interpolationData;
                double t = (double)nSubIndex / (double)(nCount + 1);

                interpolationData.m_Parameter = t;
                interpolationData.m_Factor = (1.0 - t) * f1 + t * f2 + sin(nSubIndex * 3.14159 / nCount) * 0.3;
                subInterpolationData.push_back(interpolationData);
            }

            subInterpolationCounts.push_back(nCount);
        };
           
        pLayer->WriteHatchDataInModelUnitsWithNonlinearFactors(nHatchProfileID, nPartID, hatchList, scaleParameterList1, scaleParameterList2, subInterpolationCounts, subInterpolationData);

    }

    pWriter->WriteToFile(sOutputFileName);
}


void outputCustomData(Lib3MF::PCustomXMLNode pXMLNode, const std::string & sCurrentPath, const std::string & sPrefix) {
    std::string sNewPath = sCurrentPath + "/" + pXMLNode->GetName ();
    std::cout << sPrefix << sNewPath << " (" << pXMLNode->GetNameSpace() << ")" << std::endl;

    uint64_t nAttributeCount = pXMLNode->GetAttributeCount();
    for (uint64_t nAttributeIndex = 0; nAttributeIndex < nAttributeCount; nAttributeIndex++) {
        auto pAttribute = pXMLNode->GetAttribute(nAttributeIndex);
        std::cout << sPrefix << "    attribute " << pAttribute->GetName () << ": " << pAttribute->GetValue () << std::endl;

    }

    auto pChildren = pXMLNode->GetChildren();
    uint64_t nChildCount = pChildren->GetNodeCount();
    for (uint64_t nChildIndex = 0; nChildIndex < nChildCount; nChildIndex++)
        outputCustomData(pChildren->GetNode(nChildIndex), sNewPath, sPrefix);
}

void readToolpathDemo(Lib3MF::PWrapper p3MFWrapper, const std::string sInputFileName)
{

    // Create empty model instance
    auto pModel = p3MFWrapper->CreateModel();

    // Create Source for reading model
    auto pSource = pModel->CreatePersistentSourceFromFile(sInputFileName);

    // Create ReaderObject
    auto pReader = pModel->QueryReader("3mf");

    // Read all attachments of a certain type into memory
    pReader->AddRelationToRead("http://mycompany.com/customattachment");

    // Read from disk
    pReader->ReadFromPersistentSource(pSource);

    // Get Attachment as string...
    auto pAttachment = pModel->FindAttachment("/mycompanydata/attachment.json");
    std::vector<uint8_t> attachmentBuffer;
    pAttachment->WriteToBuffer(attachmentBuffer);
    std::string sAttachment(attachmentBuffer.begin(), attachmentBuffer.end());
    

    std::cout << "Found custom file attachment: " << sAttachment << std::endl;


    // Go through all toolpaths (there should only be one)
    auto toolpathIterator = pModel->GetToolpaths();
    while (toolpathIterator->MoveNext()) {

        // Get Current Toolpath instance
        auto pToolpath = toolpathIterator->GetCurrentToolpath();

        uint32_t nLayerCount = pToolpath->GetLayerCount();
        double dUnits = pToolpath->GetUnits();

        std::cout << "Toolpath found. " << nLayerCount << " layers, Units: " << dUnits << std::endl;

        // Read global custom data...
        uint32_t nGlobalCustomDataCount = pToolpath->GetCustomDataCount();
        for (uint32_t nCustomDataIndex = 0; nCustomDataIndex < nGlobalCustomDataCount; nCustomDataIndex++) {
            auto pCustomData = pToolpath->GetCustomData(nCustomDataIndex);
            outputCustomData(pCustomData->GetRootNode (), "", "  ");
        }

        // Iterate through all layers
        for (uint32_t nLayerIndex = 0; nLayerIndex < nLayerCount; nLayerIndex++) {

            // Load Layer
            auto pLayerData = pToolpath->ReadLayerData(nLayerIndex);


            // Output layer info
            uint32_t nZMin = pToolpath->GetLayerZMin(nLayerIndex);
            uint32_t nZMax = pToolpath->GetLayerZMax(nLayerIndex);
            uint32_t nSegmentCount = pLayerData->GetSegmentCount();
            std::cout << "- layer " << nLayerIndex << " ranging from " << nZMin << " to " << nZMax << std::endl;

            // Read layer custom data...
            uint32_t nLayerCustomDataCount = pLayerData->GetCustomDataCount();
            for (uint32_t nCustomDataIndex = 0; nCustomDataIndex < nLayerCustomDataCount; nCustomDataIndex++) {
                auto pCustomData = pLayerData->GetCustomData(nCustomDataIndex);
                outputCustomData(pCustomData->GetRootNode(), "", "   ");
            }

            // iterate through all segments
            for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {

                // Get Segment Information
                uint32_t nPointCount = 0;
                Lib3MF::eToolpathSegmentType segmentType;
                pLayerData->GetSegmentInfo(nSegmentIndex, segmentType, nPointCount);

                // Get Profile Information
                auto pProfile = pLayerData->GetSegmentDefaultProfile(nSegmentIndex);

                std::string sProfileName = pProfile->GetName();
                double dLaserPower = pProfile->GetParameterDoubleValue("", "laserpower");
                double dLaserSpeed = pProfile->GetParameterDoubleValue("", "laserspeed");
                double dJumpSpeed = pProfile->GetParameterDoubleValue("", "jumpspeed");

                uint32_t nModifierCount = pProfile->GetModifierCount();
                std::cout << "modifier count: " << nModifierCount << std::endl;
                for (uint32_t nModifierIndex = 0; nModifierIndex < nModifierCount; nModifierIndex++) {
                    std::string sNameSpace;
                    std::string sValueName;
                    Lib3MF::eToolpathProfileModificationFactor modificationFactor;
                    Lib3MF::eToolpathProfileModificationType modificationType;
                    double dMin, dMax;

                    pProfile->GetModifierInformationByIndex(nModifierIndex, sNameSpace, sValueName, modificationType, modificationFactor, dMin, dMax);
                    std::cout << "- Modifier: " << sNameSpace << " / " << sValueName << ": " << (int)modificationFactor << " min: " << dMin << " to " << dMax << std::endl;
                }


                // Write segment information and retrieve point data
                switch (segmentType) {
                case Lib3MF::eToolpathSegmentType::Loop: {
                    std::vector <Lib3MF::sPosition2D> pointData;
                    pLayerData->GetSegmentPointDataInModelUnits(nSegmentIndex, pointData);

                    std::vector<double> factorValues;

                    pLayerData->GetSegmentPointModificationFactors (nSegmentIndex, Lib3MF::eToolpathProfileModificationFactor::FactorF, factorValues);
                    std::cout << "    o Loop: " << nPointCount << " points, Profile: " + sProfileName << " " << dLaserPower << "W, " << dLaserSpeed << "mm/s" << std::endl;
                        
                    for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
                        auto& point = pointData.at(nPointIndex);
                        std::cout << "  Point: " << point.m_Coordinates[0] << "/" << point.m_Coordinates[1] << ": " << factorValues[nPointIndex] << std::endl;
                    }

                    break;
                }

                case Lib3MF::eToolpathSegmentType::Polyline: {
                    std::vector <Lib3MF::sPosition2D> pointData;
                    pLayerData->GetSegmentPointDataInModelUnits(nSegmentIndex, pointData);

                    std::vector<double> factorValues;
                    pLayerData->GetSegmentPointModificationFactors(nSegmentIndex, Lib3MF::eToolpathProfileModificationFactor::FactorF, factorValues);

                    std::cout << "    o Polyline: " << nPointCount << " points, Profile: " + sProfileName << " " << dLaserPower << "W, " << dLaserSpeed << "mm/s" << std::endl;
                    for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
                        auto& point = pointData.at(nPointIndex);
                        std::cout << "  Point: " << point.m_Coordinates[0] << "/" << point.m_Coordinates[1] << ": " << factorValues[nPointIndex] << std::endl;
                    }
                    break;
                }

                case Lib3MF::eToolpathSegmentType::Hatch: {
                    std::cout << "    o Hatches: " << nPointCount << " points, Profile: " + sProfileName << " " << dLaserPower << "W, " << dLaserSpeed << "mm/s" << std::endl;

                    std::vector <Lib3MF::sHatch2D> hatchData;
                    pLayerData->GetSegmentHatchDataInModelUnits(nSegmentIndex, hatchData);


                    std::vector<Lib3MF::sHatch2DFactors> factorValues;
                    pLayerData->GetLinearSegmentHatchModificationFactors(nSegmentIndex, Lib3MF::eToolpathProfileModificationFactor::FactorF, factorValues);
                    uint32_t nHatchCount = nPointCount / 2;

					bool bHasNonlinearOverrides = pLayerData->SegmentHasNonlinearHatchModificationInterpolation(nSegmentIndex);

/*                    for (uint32_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
                        auto& hatch = hatchData.at(nHatchIndex);
                        std::cout << "  hatch: " << hatch.m_Point1Coordinates[0] << "/" << hatch.m_Point1Coordinates[1] << " - " << hatch.m_Point2Coordinates[0] << "/" << hatch.m_Point2Coordinates[1]  << ": " << factorValues[nHatchIndex].m_Point1Override << " - " << factorValues[nHatchIndex].m_Point2Override << std::endl;

                        if (bHasNonlinearOverrides) {
							std::vector<Lib3MF::sHatchOverrideInterpolationData> subInterpolationData;
							pLayerData->GetSegmentNonlinearHatchOverrideInterpolation(nSegmentIndex, nHatchIndex, Lib3MF::eToolpathProfileOverrideFactor::FactorF, subInterpolationData);
							for (uint32_t nSubIndex = 0; nSubIndex < subInterpolationData.size (); nSubIndex++) {
								auto& subData = subInterpolationData.at(nSubIndex);
								std::cout << "    sub " << nSubIndex << ": " << subData.m_Parameter << " - " << subData.m_Override << std::endl;
							}
                        }
                    } */
                    break;
                }
                }
            }

            std::cout << std::endl;
        }
    } 

}


int main()
{
    try {
        auto p3MFWrapper = Lib3MF::CWrapper::loadLibrary("lib3mf_win64.dll");

        std::cout << "Writing dummy.toolpath.3mf" << std::endl;
        writeToolpathDemo(p3MFWrapper, "dummy.toolpath.3mf");

        std::cout << "Reading dummy.toolpath.3mf" << std::endl;
        readToolpathDemo(p3MFWrapper, "dummy.toolpath.3mf");

    }
    catch (std::exception& E) {
        std::cout << "fatal error: " << E.what() << std::endl;
    }
    
}

