/*
 * carmageddon_resource.h
 *
 *  Created on: 15 Jul 2017
 *      Author: gijs
 */

#ifndef SRC_REST_CARMAGEDDON_RESOURCE_H_
#define SRC_REST_CARMAGEDDON_RESOURCE_H_

#include <restbed>
#include <string>
#include <list>
#include <memory>

class carmageddon_resource {
public:
	virtual ~carmageddon_resource() {}
	void sendError(const std::shared_ptr<restbed::Session> session, std::string msg) {
		const std::string body = "{\"error\": \"" + msg + "\"}";
		session->close(500, body, {
				{ "Content-Type", "application/json" },
				{ "Content-Length", std::to_string(body.size()) }
		});
	}
	virtual std::list<std::shared_ptr<restbed::Resource>> getResources() = 0;
};

#endif /* SRC_REST_CARMAGEDDON_RESOURCE_H_ */
