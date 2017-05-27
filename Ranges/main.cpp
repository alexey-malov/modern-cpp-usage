#include <vector>
#include <string>
#include <iterator>
#include <functional>
#include <iostream>
#include <sstream>
#define BOOST_RESULT_OF_USE_DECLTYPE
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/formatted.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/range/algorithm/sort.hpp>

using namespace std;
using namespace boost::adaptors;
using boost::copy;
using boost::max_element;
using boost::sort;
using boost::transform;

enum class Gender : uint8_t { Male, Female };

struct Person {
	string name;
	short age;
	Gender gender;
};

ostream& operator<<(ostream& out, const Person& p)
{
	out << p.name 
		<< " (age " << p.age << ", " 
		<< (p.gender == Gender::Male ? "male" : "female") 
		<< ")";
	return out;
}

const vector<Person> people = {
	{ "Ivan", 25, Gender::Male },
	{ "Peter", 35, Gender::Male },
	{ "Masha", 24, Gender::Female },
	{ "Dasha", 33, Gender::Female },
	{ "Ivan", 37, Gender::Male },
	{ "Sveta", 28, Gender::Female },
	{ "Ivan", 20, Gender::Male },
	{ "Constantine", 23, Gender::Male },
	{ "Eugene", 35, Gender::Male },
};

void WriteToStdout() {
	for (size_t i = 0; i < people.size(); ++i) {
		cout << people[i] << "\n";
	}

	for (auto & person : people) {
		cout << person << "\n";
	}

	copy(people, ostream_iterator<Person>(cout, "\n"));
}

void FindTheOldestOne() {
	// Raw loop version
	{
		if (!people.empty()) {
			size_t oldest = 0;
			for (size_t i = 1; i < people.size(); ++i) {
				if (people[i].age > people[oldest].age) {
					oldest = i;
				}
			}
			cout << "The oldest one is " << people[oldest] << "\n";
		}
		else {
			cout << "No people\n";
		}
	}

	// max_element
	{
		auto orderedByAge = [](auto & lhs, auto & rhs) { 
			return lhs.age < rhs.age; 
		};
		auto oldest = max_element(people, orderedByAge);
		if (oldest != people.end()) 
			cout << "The oldest one is " << *oldest << "\n";
		else
			cout << "No people\n";
	}
}

// Print all women aged between 20 and 30,
// then print all men aged between 25 and 40
void SelectPeopleByAgeAndGender() {
	cout << "---Raw loop---\n";
	{
		for (size_t i = 0; i < people.size(); ++i) {
			if (people[i].gender == Gender::Female &&
				people[i].age >= 20 && people[i].age <= 30) {
				cout << people[i] << "\n";
			}
		}
		for (size_t i = 0; i < people.size(); ++i) {
			if (people[i].gender == Gender::Male &&
				people[i].age >= 25 && people[i].age <= 40) {
				cout << people[i] << "\n";
			}
		}
	}
	cout << "---Range-based for---\n";
	{
		for (auto& person : people) {
			if ((person.gender == Gender::Female) && 
				(person.age >= 20 && person.age <= 30)) {
				cout << person << "\n";
			}
		}
		for (auto& person : people) {
			if ((person.gender == Gender::Male) && 
				(person.age >= 25 && person.age <= 40)) {
				cout << person << "\n";
			}
		}
	}
	cout << "---Range filtering---\n";
	{
		auto ByGenderAndAge = [](Gender g, int minAge, int maxAge) {
			return [=](auto& p) { 
				return (p.gender == g) && p.age >= minAge && p.age <= maxAge; 
			};
		};

		auto ToStdout = ostream_iterator<Person>(cout, "\n");
		copy(people | filtered(ByGenderAndAge(Gender::Female, 20, 30)), ToStdout);
		copy(people | filtered(ByGenderAndAge(Gender::Male, 25, 40)), ToStdout);
	}
}

decltype(auto) ToRef()
{
	return [](auto && value) { return ref(value); };
}

void Indexing()
{
	cout << "Sorted people with initial indices\n";
	{
		using IndexedPerson = pair<Person, size_t>;
		vector<IndexedPerson> indexedPeople;

		for (size_t i = 0; i < people.size(); ++i) {
			indexedPeople.emplace_back(people[i], i + 1);
		}
		sort(indexedPeople.begin(), indexedPeople.end(), [](auto& lhs, auto & rhs) { 
			return lhs.first.name < rhs.first.name; 
		});
		for (size_t i = 0; i < indexedPeople.size(); ++i) {
			cout << indexedPeople[i].second << ". " << indexedPeople[i].first << "\n";
		}
	}
	cout << "Sorted people with initial indices\n";
	{
		using ConstPersonRef = reference_wrapper<const Person>;

		auto indexedPeople = people | transformed(ToRef()) | indexed(1);
		using IndexedPersonRef = decltype(indexedPeople)::value_type;

		vector<IndexedPersonRef> peopleToSort(indexedPeople.begin(), indexedPeople.end());

		auto ByName = [](auto & lhs, auto & rhs) { return lhs.value().get().name < rhs.value().get().name; };
		sort(peopleToSort, ByName);

		auto ToStringWithIndex = [](auto & person) {
			ostringstream out; out << person.index() << ". " << person.value(); return out.str();
		};

		boost::transform(peopleToSort, ostream_iterator<string>(cout, "\n"), ToStringWithIndex);
	}
}

int main()
{
	//WriteToStdout();
	//FindTheOldestOne();
	//SelectPeopleByAgeAndGender();
	Indexing();
/*

	auto ageInRange = [](int minAge, int maxAge) {
		return [=](const Person & p) { return p.age >= minAge && p.age < maxAge; };
	};

	auto genderIs = [](Gender g) {
		return [g](const Person& p) { return p.gender == g; };
	};

	auto selectedPeople = people
		| filtered(ageInRange(16, 32))
		| filtered(genderIs(Gender::Female));

	copy(selectedPeople, ostream_iterator<Person>(cout, "\n"));

	for (auto & person : people) {
		if ((person.age >= 16 && person.age < 32) &&
			(person.gender == Gender::Female)) {
			cout << person << "\n";
		}
	}*/
}