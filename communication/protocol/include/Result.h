/*
 * ContainerOperationResult.h
 *
 *  Created on: Jun 20, 2018
 *      Author: xgallom
 */

#ifndef CONTAINER_RESULT_H_
#define CONTAINER_RESULT_H_

namespace Container
{
	template<typename T>
	struct Result {
		 Result() : isValid(false) {}
		 Result(const T &value) : isValid(true), value(value) {}

		bool isValid;
		T value;
	};
}

#endif /* CONTAINER_RESULT_H_ */