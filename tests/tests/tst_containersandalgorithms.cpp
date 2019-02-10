#include <QtTest>
//#include <SegmentSet/segment_set.hpp>

// add necessary includes here

class ContainersAndAlgorithms : public QObject
{
   Q_OBJECT

public:
   ContainersAndAlgorithms();
   ~ContainersAndAlgorithms();

private slots:
   void initTestCase();
   void cleanupTestCase();
   void test_case1();

};

ContainersAndAlgorithms::ContainersAndAlgorithms()
{

}

ContainersAndAlgorithms::~ContainersAndAlgorithms()
{

}

void ContainersAndAlgorithms::initTestCase()
{

}

void ContainersAndAlgorithms::cleanupTestCase()
{

}

void ContainersAndAlgorithms::test_case1()
{

}

QTEST_APPLESS_MAIN(ContainersAndAlgorithms)

#include "tst_containersandalgorithms.moc"
