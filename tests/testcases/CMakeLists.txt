CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1)
CMAKE_POLICY(VERSION 2.8.1)

SET(PROJECT_NAME Testcases)
PROJECT(${PROJECT_NAME})
SET(TESTCASES_PROJECT ${PROJECT_NAME} PARENT_SCOPE)

FILE(GLOB SOURCES ${TEST_DIR}/testcases/*/*.cpp ${TEST_DIR}/testcases/main.cpp)

INCLUDE_DIRECTORIES(${TEST_LIBRARIES_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SEMI_INTERNAL_LIBRARIES_INCLUDES})
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${INTERNAL_LIBRARIES} ${SEMI_INTERNAL_LIBRARIES} ${EXTERNAL_LIBRARIES} ${TEST_LIBRARIES})

MESSAGE(STATUS "Building testcase project")
