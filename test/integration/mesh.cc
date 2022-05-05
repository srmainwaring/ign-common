/*
 * Copyright (C) 2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <gtest/gtest.h>

#include "ignition/common/Material.hh"
#include "ignition/common/Mesh.hh"
#include "ignition/common/Skeleton.hh"
#include "ignition/common/SubMesh.hh"
#include "ignition/common/MeshManager.hh"
#include "ignition/common/SystemPaths.hh"
#include "ignition/math/Vector3.hh"

#include <ignition/common/testing/AutoLogFixture.hh>
#include <ignition/common/testing/TestPaths.hh>

using namespace ignition;

class MeshTest : public common::testing::AutoLogFixture { };

constexpr const char asciiSTLBox[] =
R"(solid MYSOLID
  facet normal  0.0   0.0  -1.0
    outer loop
      vertex    0.0   0.0   0.0
      vertex    1.0   1.0   0.0
      vertex    1.0   0.0   0.0
    endloop
  endfacet
  facet normal  0.0   0.0  -1.0
    outer loop
      vertex    0.0   0.0   0.0
      vertex    0.0   1.0   0.0
      vertex    1.0   1.0   0.0
    endloop
  endfacet
  facet normal -1.0   0.0   0.0
    outer loop
      vertex    0.0   0.0   0.0
      vertex    0.0   1.0   1.0
      vertex    0.0   1.0   0.0
    endloop
  endfacet
  facet normal -1.0   0.0   0.0
    outer loop
      vertex    0.0   0.0   0.0
      vertex    0.0   0.0   1.0
      vertex    0.0   1.0   1.0
    endloop
  endfacet
  facet normal  0.0   1.0   0.0
    outer loop
      vertex    0.0   1.0   0.0
      vertex    1.0   1.0   1.0
      vertex    1.0   1.0   0.0
    endloop
  endfacet
  facet normal  0.0   1.0   0.0
    outer loop
      vertex    0.0   1.0   0.0
      vertex    0.0   1.0   1.0
      vertex    1.0   1.0   1.0
    endloop
  endfacet
  facet normal  1.0   0.0   0.0
    outer loop
      vertex    1.0   0.0   0.0
      vertex    1.0   1.0   0.0
      vertex    1.0   1.0   1.0
    endloop
  endfacet
  facet normal  1.0   0.0   0.0
    outer loop
      vertex    1.0   0.0   0.0
      vertex    1.0   1.0   1.0
      vertex    1.0   0.0   1.0
    endloop
  endfacet
  facet normal  0.0  -1.0   0.0
    outer loop
      vertex    0.0   0.0   0.0
      vertex    1.0   0.0   0.0
      vertex    1.0   0.0   1.0
    endloop
  endfacet
  facet normal  0.0  -1.0   0.0
    outer loop
      vertex    0.0   0.0   0.0
      vertex    1.0   0.0   1.0
      vertex    0.0   0.0   1.0
    endloop
  endfacet
  facet normal  0.0   0.0   1.0
    outer loop
      vertex    0.0   0.0   1.0
      vertex    1.0   0.0   1.0
      vertex    1.0   1.0   1.0
    endloop
  endfacet
  facet normal  0.0   0.0   1.0
    outer loop
      vertex    0.0   0.0   1.0
      vertex    1.0   1.0   1.0
      vertex    0.0   1.0   1.0
    endloop
  endfacet
endsolid MYSOLID)";


/////////////////////////////////////////////////
TEST_F(MeshTest, Load)
{
  // Load things that we know are invalid extensions
  EXPECT_EQ(nullptr, common::MeshManager::Instance()->Load("break.mesh"));
  EXPECT_EQ(nullptr, common::MeshManager::Instance()->Load("break.3ds"));
  EXPECT_EQ(nullptr, common::MeshManager::Instance()->Load("break.xml"));

  common::systemPaths()->AddFilePaths(common::testing::TestFile("data"));

  // Loading should be successful
  EXPECT_NE(nullptr, common::MeshManager::Instance()->Load("box.dae"));
  EXPECT_NE(nullptr, common::MeshManager::Instance()->Load("box.obj"));

  // Reloading should not cause errors
  EXPECT_NE(nullptr, common::MeshManager::Instance()->Load("box.dae"));
  EXPECT_NE(nullptr, common::MeshManager::Instance()->Load("box.obj"));
}

/////////////////////////////////////////////////
TEST_F(MeshTest, Access)
{
  auto mesh = common::MeshManager::Instance()->MeshByName("unit_box");
  ASSERT_NE(nullptr, mesh);
  EXPECT_EQ(24u, mesh->VertexCount());
  EXPECT_EQ(24u, mesh->NormalCount());
  EXPECT_EQ(36u, mesh->IndexCount());
  EXPECT_EQ(24u, mesh->TexCoordCount());
  EXPECT_EQ(0u, mesh->MaterialCount());

  ignition::math::Vector3d center, min, max;
  mesh->AABB(center, min, max);
  EXPECT_TRUE(center == ignition::math::Vector3d(0, 0, 0));
  EXPECT_TRUE(min == ignition::math::Vector3d(-.5, -.5, -.5));
  EXPECT_TRUE(max == ignition::math::Vector3d(.5, .5, .5));

  double *vertArray = nullptr;
  int *indArray = nullptr;
  mesh->FillArrays(&vertArray, &indArray);

  int i = 0;
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);

  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);

  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);

  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);

  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);

  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);

  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);

  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(.5, vertArray[i++]);
  EXPECT_FLOAT_EQ(-.5, vertArray[i++]);

  delete vertArray;
  delete indArray;

  EXPECT_FALSE(common::MeshManager::Instance()->HasMesh(""));
  EXPECT_TRUE(common::MeshManager::Instance()->HasMesh("unit_box"));

  mesh = common::MeshManager::Instance()->MeshByName("foo_box_doesnt_exist");
  EXPECT_EQ(nullptr, mesh);
  EXPECT_FALSE(
      common::MeshManager::Instance()->RemoveMesh("foo_box_doesnt_exist"));
  EXPECT_TRUE(common::MeshManager::Instance()->RemoveMesh("unit_box"));
}

/////////////////////////////////////////////////
TEST_F(MeshTest, RoundtripStl)
{
  std::ofstream stlFile("ignition_stl_test.stl", std::ios::out);
  stlFile << asciiSTLBox;
  stlFile.close();

  auto mesh =
    common::MeshManager::Instance()->Load("ignition_stl_test-bad.stl");
  EXPECT_EQ(nullptr, mesh);

  common::systemPaths()->AddFilePaths(common::cwd());
  mesh = common::MeshManager::Instance()->Load("ignition_stl_test.stl");
  ASSERT_NE(nullptr, mesh);

  math::Vector3d center, min, max;
  mesh->AABB(center, min, max);
  EXPECT_TRUE(center == ignition::math::Vector3d(0.5, 0.5, 0.5));
  EXPECT_TRUE(min == ignition::math::Vector3d(0, 0, 0));
  EXPECT_TRUE(max == ignition::math::Vector3d(1, 1, 1));
}

/////////////////////////////////////////////////
TEST_F(MeshTest, Export)
{
  std::ofstream stlFile("ignition_stl_test.stl", std::ios::out);
  stlFile << asciiSTLBox;
  stlFile.close();

  common::systemPaths()->AddFilePaths(common::cwd());
  auto mesh = common::MeshManager::Instance()->Load("ignition_stl_test.stl");

  ASSERT_NE(nullptr, mesh);
  common::MeshManager::Instance()->Export(mesh,
      common::joinPaths(common::cwd(), "ignition_stl_test2"), "stl", false);
  common::MeshManager::Instance()->Export(mesh,
      common::joinPaths(common::cwd(), "ignition_stl_test2"), "dae", false);

  EXPECT_FALSE(common::exists("ignition_stl_test2.stl"));
  EXPECT_TRUE(common::exists("ignition_stl_test2.dae"));
}

