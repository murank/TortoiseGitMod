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

#include "WorkerThread.h"
#include "Task.h"

using namespace ::testing;

namespace {

class MockWorkerThread : public WorkerThread {
public:

	MockWorkerThread() : WorkerThread(NULL) {}
	
	MOCK_METHOD1(DoWait, bool(AutoLocker& lock));
	MOCK_METHOD0(DoNotify, void());
	MOCK_METHOD0(NotifyTaskFinished, void());

	using WorkerThread::DoRun;
};

class MockTask : public Task {
public:

	MOCK_METHOD0(Run, void());
	MOCK_METHOD0(OnCancelRequired, void());

};

} // namespace

TEST(WorkerThread, AssignTask)
{
	MockWorkerThread mwt;

	EXPECT_FALSE(mwt.HasTask());


	shared_ptr<MockTask> mt(new MockTask);

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);

	EXPECT_TRUE(mwt.AssignTask(mt));
	EXPECT_TRUE(mwt.HasTask());

	EXPECT_CALL(mwt, DoNotify())
		.Times(0);

	EXPECT_FALSE(mwt.AssignTask(mt));
	EXPECT_TRUE(mwt.HasTask());

	EXPECT_CALL(*mt, OnCancelRequired())
		.Times(1);
}

TEST(WorkerThread, AssignTaskAfterCancellation)
{
	MockWorkerThread mwt;

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);

	mwt.CancelAsync();

	shared_ptr<Task> mt(new MockTask);

	EXPECT_CALL(mwt, DoNotify())
		.Times(0);

	EXPECT_FALSE(mwt.AssignTask(mt));
	EXPECT_FALSE(mwt.HasTask());
}

namespace {

class TaskAssigner : public std::unary_function<void, void> {
public:
	TaskAssigner(WorkerThread* target, const shared_ptr<Task>& task)
		: m_target(target), m_task(task)
	{}
	void operator()() {
		m_target->AssignTask(m_task);
	}
private:
	WorkerThread* m_target;
	shared_ptr<Task> m_task;
};

} // namespace

TEST(WorkerThread, DoRunWillConsumeTask)
{
	MockWorkerThread mwt;
	shared_ptr<MockTask> mt(new MockTask);

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);

	mwt.AssignTask(mt);

	EXPECT_TRUE(mwt.HasTask());

	EXPECT_CALL(mwt, DoWait(_))
		.Times(0);
	EXPECT_CALL(mwt, NotifyTaskFinished())
		.Times(1);

	EXPECT_CALL(*mt, Run())
		.Times(1);

	mwt.DoRun();

	EXPECT_FALSE(mwt.HasTask());
}

TEST(WorkerThread, AssignTaskWhileDoRun)
{
	MockWorkerThread mwt;
	shared_ptr<MockTask> mt(new MockTask);

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);
	EXPECT_CALL(mwt, DoWait(_))
		.Times(1)
		.WillOnce(DoAll(InvokeWithoutArgs(TaskAssigner(&mwt, mt)), Return(true)));
	EXPECT_CALL(mwt, NotifyTaskFinished())
		.Times(1);

	EXPECT_CALL(*mt, Run())
		.Times(1);

	mwt.DoRun();
}

TEST(WorkerThread, CancelWhileDoRun)
{
	MockWorkerThread mwt;

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);
	EXPECT_CALL(mwt, DoWait(_))
		.Times(1)
		.WillOnce(DoAll(IgnoreResult(InvokeWithoutArgs(&mwt, &MockWorkerThread::CancelAsync)), Return(true)));
	EXPECT_CALL(mwt, NotifyTaskFinished())
		.Times(0);

	mwt.DoRun();
}

TEST(WorkerThread, CancelAfterAssignTask)
{
	MockWorkerThread mwt;
	shared_ptr<MockTask> mt(new MockTask);

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);

	EXPECT_TRUE(mwt.AssignTask(mt));

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);
	EXPECT_CALL(*mt, OnCancelRequired())
		.Times(1);

	EXPECT_TRUE(mwt.CancelAsync());
}

TEST(WorkerThread, DoRunWithWaitFailure)
{
	MockWorkerThread mwt;
	Sequence s;

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);
	EXPECT_CALL(mwt, DoWait(_))
		.Times(1)
		.InSequence(s)
		.WillOnce(Return(false));
	EXPECT_CALL(mwt, DoWait(_))
		.Times(1)
		.InSequence(s)
		.WillOnce(DoAll(IgnoreResult(InvokeWithoutArgs(&mwt, &MockWorkerThread::CancelAsync)), Return(true)));
	EXPECT_CALL(mwt, NotifyTaskFinished())
		.Times(0);

	mwt.DoRun();
}

TEST(WorkerThread, DoRunWithException)
{
	MockWorkerThread mwt;
	shared_ptr<MockTask> mt(new MockTask);

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);
	EXPECT_CALL(mwt, DoWait(_))
		.Times(1)
		.WillOnce(DoAll(InvokeWithoutArgs(TaskAssigner(&mwt, mt)), Return(true)));
	EXPECT_CALL(mwt, NotifyTaskFinished())
		.Times(1);

	EXPECT_CALL(*mt, Run())
		.Times(1)
		.WillOnce(Throw(std::runtime_error("some error")));

	mwt.DoRun();
}

TEST(WorkerThread, DoRunAfterCancellation)
{
	MockWorkerThread mwt;

	EXPECT_CALL(mwt, DoNotify())
		.Times(1);
	EXPECT_CALL(mwt, DoWait(_))
		.Times(0);
	EXPECT_CALL(mwt, NotifyTaskFinished())
		.Times(0);

	EXPECT_TRUE(mwt.CancelAsync());
	mwt.DoRun();
}
