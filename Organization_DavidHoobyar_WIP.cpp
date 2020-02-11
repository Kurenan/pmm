#include <iostream>
#include <vector>
using namespace std;

class Step
{
public:
	bool IsActive;
	virtual int runStep() = 0;

};

class Module
{
public:
	int StatusCode;
	vector<Step*> StepList()
	{};
};

class TransInit : public Step
{
public:
	bool IsActive;
	int runStep()
	{
		//Do stuff
		return 1;
	}
};

class TransUpdate : public Step
{
public:
	bool IsActive;
	int runStep()
	{
		//Do stuff
		return 2;
	}
};

class Transponder : public Module
{
public:	
	int StatusCode;
	vector<Step*> StepList;
	Transponder()
	{
		vector<Step*> stepList;
		Step* step1 = new TransInit();
		Step* step2 = new TransUpdate();

		stepList.push_back(step1);
		stepList.push_back(step2);
		StepList = stepList;
	}
};

class Loadout
{
public:
	vector<Module*> moduleList;
	void AddModule(Module& module, vector<bool> stepsToActivate)
	{
		
		vector<Step*> stepList = module.StepList;

		//Make sure there are the same number of elements in the step list as the list of bools
		if (stepList.size() == stepsToActivate.size())
		{
			vector<Step*> stepListNew;

			//Iterate through the step list and use "i" as the index for checking whether or not to keep the item
			for (int i = 0; i < (int)stepList.size(); i++)
			{
				if (stepsToActivate.at(i))
				{
					stepListNew.push_back(stepList.at(i));
				}
			}
			//What we're trying to do here is replace the existing module->StepList with the new one we've created.
			//We need to learn more about pointers to do this.
			//module->StepList == stepListNew;
			//moduleList.push_back(module);
		}

	}
};

int main()
{
	Loadout loadout;
	Transponder transponder;
	Module& module = transponder;
	//Module* module = new Transponder;
	vector<bool> stepsToActivate{ false, true };
	loadout.AddModule(module, stepsToActivate);
}
