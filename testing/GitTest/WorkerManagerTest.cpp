// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2011 - TortoiseGit

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "StdAfxTest.h"

#include "WorkerManager.h"

#include "WorkerThread.h"

using namespace ::testing;

namespace {

class MockWorkerManager : public WorkerManager {
public:

	MOCK_CONST_METHOD0(CreateWorker, shared_ptr<WorkerThread>());

	using WorkerManager::RegisterActiveWorker;
	using WorkerManager::RegisterInactiveWorker;
};

class MockWorkerThread : public WorkerThread {
public:

	MockWorkerThread() : WorkerThread(NULL) {}
	
	MOCK_METHOD1(AssignTask, bool(const shared_ptr<Task>& task));
	MOCK_METHOD0(OnCancelRequired, void());
};

} // namespace

TEST(WorkerManager, Initialize)
{
	MockWorkerManager mwm;

	EXPECT_EQ(0, mwm.GetMaxWorkers());

	std::pair<size_t, size_t> num = mwm.GetNumWorkers();
	EXPECT_EQ(0, num.first);
	EXPECT_EQ(0, num.second);

	mwm.Initialize(99);

	EXPECT_EQ(99, mwm.GetMaxWorkers());

	num = mwm.GetNumWorkers();
	EXPECT_EQ(0, num.first);
	EXPECT_EQ(0, num.second);
}

TEST(WorkerManager, RegisterTask)
{
	shared_ptr<MockWorkerThread> mwt(new MockWorkerThread);

	EXPECT_CALL(*mwt, AssignTask(_))
		.WillOnce(Return(true));

	MockWorkerManager mwm;

	EXPECT_CALL(mwm, CreateWorker())
		.WillOnce(Return(mwt));

	mwm.RegisterTask(shared_ptr<Task>());

	std::pair<size_t, size_t> num = mwm.GetNumWorkers();
	EXPECT_EQ(1, num.first);
	EXPECT_EQ(0, num.second);

	EXPECT_CALL(*mwt, OnCancelRequired())
		.Times(1);
}

TEST(WorkerManager, RegisterTaskToInactiveWorker)
{
	shared_ptr<MockWorkerThread> mwt(new MockWorkerThread);
	MockWorkerManager mwm;

	EXPECT_CALL(*mwt, AssignTask(_))
		.WillOnce(Return(true));

	EXPECT_CALL(mwm, CreateWorker())
		.Times(0);

	mwm.RegisterInactiveWorker(mwt);

	std::pair<size_t, size_t> num = mwm.GetNumWorkers();
	EXPECT_EQ(0, num.first);
	EXPECT_EQ(1, num.second);

	mwm.RegisterTask(shared_ptr<Task>());

	num = mwm.GetNumWorkers();
	EXPECT_EQ(1, num.first);
	EXPECT_EQ(0, num.second);

	EXPECT_CALL(*mwt, OnCancelRequired())
		.Times(1);
}

TEST(WorkerManager, RegisterTaskWithAssignTaskFailure)
{
	MockWorkerManager mwm;
	shared_ptr<MockWorkerThread> mwt(new MockWorkerThread);

	EXPECT_CALL(*mwt, AssignTask(_))
		.WillOnce(Return(false))
		.WillOnce(Return(true));

	EXPECT_CALL(mwm, CreateWorker())
		.Times(2)
		.WillRepeatedly(Return(mwt));

	mwm.RegisterTask(shared_ptr<Task>());

	std::pair<size_t, size_t> num = mwm.GetNumWorkers();
	EXPECT_EQ(1, num.first);
	EXPECT_EQ(0, num.second);

	EXPECT_CALL(*mwt, OnCancelRequired())
		.Times(1);
}

TEST(WorkerManager, RegisterTaskWithAssignTaskFailureAndCancellation)
{
	MockWorkerManager mwm;
	shared_ptr<MockWorkerThread> mwt(new MockWorkerThread);

	EXPECT_CALL(*mwt, AssignTask(_))
		.WillOnce(DoAll(IgnoreResult(InvokeWithoutArgs(&mwm, &MockWorkerManager::CancelAsync)), Return(false)));

	EXPECT_CALL(mwm, CreateWorker())
		.WillOnce(Return(mwt));

	mwm.RegisterTask(shared_ptr<Task>());

	std::pair<size_t, size_t> num = mwm.GetNumWorkers();
	EXPECT_EQ(0, num.first);
	EXPECT_EQ(0, num.second);
}

TEST(WorkerManager, RegisterTaskAfterCancellation)
{
	MockWorkerManager mwm;

	mwm.CancelAsync();

	mwm.RegisterTask(shared_ptr<Task>());

	std::pair<size_t, size_t> num = mwm.GetNumWorkers();
	EXPECT_EQ(0, num.first);
	EXPECT_EQ(0, num.second);
}

TEST(WorkerManager, Cancellation)
{
	MockWorkerManager mwm;
	shared_ptr<MockWorkerThread> mwt1(new MockWorkerThread);
	shared_ptr<MockWorkerThread> mwt2(new MockWorkerThread);

	EXPECT_CALL(*mwt1, OnCancelRequired())
		.Times(1);
	EXPECT_CALL(*mwt2, OnCancelRequired())
		.Times(1);

	mwm.RegisterActiveWorker(mwt1);
	mwm.RegisterInactiveWorker(mwt2);

	mwm.CancelAsync();
}

namespace {

class MockWorkerThread2 : public MockWorkerThread {
public:

	virtual ~MockWorkerThread2()
	{
		OnDestroy();
	}

	MOCK_METHOD0(OnDestroy, void());
};

} // namespace

TEST(WorkerManager, OnFinishTask)
{
	MockWorkerManager mwm;
	MockWorkerThread2* mwt1 = new MockWorkerThread2;
	MockWorkerThread2* mwt2 = new MockWorkerThread2;
	MockWorkerThread2* mwt3 = new MockWorkerThread2;

	mwm.RegisterActiveWorker(shared_ptr<MockWorkerThread2>(mwt1));
	mwm.RegisterActiveWorker(shared_ptr<MockWorkerThread2>(mwt2));
	mwm.RegisterActiveWorker(shared_ptr<MockWorkerThread2>(mwt3));

	MockFunction<void(const TCHAR* check_point_name)> check;

	Expectation check1 = EXPECT_CALL(check, Call(_T("1")));
	EXPECT_CALL(*mwt1, OnDestroy())
		.Times(1)
		.After(check1);

	Expectation check2 = EXPECT_CALL(check, Call(_T("2")));
	EXPECT_CALL(*mwt2, OnDestroy())
		.Times(1)
		.After(check2);

	Expectation check3 = EXPECT_CALL(check, Call(_T("3")));
	EXPECT_CALL(*mwt3, OnDestroy())
		.Times(1)
		.After(check3);

	std::pair<size_t, size_t> num = mwm.GetNumWorkers();
	EXPECT_EQ(3, num.first);
	EXPECT_EQ(0, num.second);

	mwm.Initialize(1);

	mwm.OnFinishTask(mwt2);
	num = mwm.GetNumWorkers();
	EXPECT_EQ(2, num.first);
	EXPECT_EQ(0, num.second);

	check.Call(_T("2"));

	mwm.OnFinishTask(mwt1);
	num = mwm.GetNumWorkers();
	EXPECT_EQ(1, num.first);
	EXPECT_EQ(0, num.second);

	check.Call(_T("1"));

	mwm.OnFinishTask(mwt3);
	num = mwm.GetNumWorkers();
	EXPECT_EQ(0, num.first);
	EXPECT_EQ(1, num.second);

	check.Call(_T("3"));

	EXPECT_CALL(*mwt3, OnCancelRequired())
		.Times(1);
}
