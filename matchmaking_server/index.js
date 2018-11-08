const express = require('express')
const ipInt = require('ip-to-int')
const validator = require('validator')

const MongoClient = require('mongodb').MongoClient
const app = express()

const port = 8080
const maxTime = (2 * 1000 * 60) // 2 minutes
const url = 'mongodb://localhost:27017'
const dbName = 'codonline'

let db = null
let clientList = []
let partyRooms = []

let client_nbr = 0;
let party_nbr = 0;

///////////////////////////
//  Partie serveur PS4
///////////////////////////

const changeEndianness = (string) => {
        const result = []
        let len = string.length - 2
        while (len >= 0) {
          result.push(string.substr(len, 2))
          len -= 2
        }
        return result.join('')
}

const hex32LE = (int) => {
	let hex = int.toString(16)
	while (hex.length < 8) {
		hex = "0" + hex
	}
	return changeEndianness ( hex )
}

const hex32 = (int) => {
	let hex = int.toString(16)
	while (hex.length < 8) {
		hex = "0" + hex
	}
	return hex
}

const findUser = (identifier, callback) => {
	let users = db.collection('users')

	users.find({ 'identifier' : identifier }).toArray(function(err, docs) {
		callback(docs[0])
	})
}

const usernameTaken = (username, callback) => {
	let users = db.collection('users')

	users.count({ 'username': username }, function(err, result) {
		callback(result)
	})
}

const ps4Exist = (identifier, callback) => {
	let users = db.collection('users')

	users.count({ 'identifier': identifier }, function(err, result) {
		callback(result)
	})
}

const updateIP = (identifier, ip) => {
	let users = db.collection('users')

	users.findOneAndUpdate({ identifier : { $lt: identifier } }, { $set: { last_ip: ip } }, function(err, result) {
		let current = new Date().getTime()
		console.log(`[${current}][${ip}][${identifier}] IP Updated`)
	})
}

const registerUser = (identifier, username, ip, callback) => {
	let users = db.collection('users')

	users.insert(
		{
		    "identifier" : identifier,
		    "username" : username,
		    "last_ip" : ip,
		    "color" : 0,
		    "is_ban" : false
		}, function(err, result) {
			callback(err)
		}
	);
}

const clearIP = (ip) => {
	// Local debug
	if (ip == "::1") {
		ip = "127.0.0.1"
	} 
	ip = ip.replace(/^.*:/, '')
	return ip
}

app.get('/info', (request, response) => {
	let data = {rooms: party_nbr, players: client_nbr}
	response.setHeader('Content-Type', 'application/json')
  	response.setHeader("Access-Control-Allow-Origin", "*");
  	response.setHeader("Access-Control-Allow-Headers", "X-Requested-With");
	response.send( JSON.stringify(data) )
})

app.post('/heartbeat/:partyid', (request, response) => {

    let identifier = ''

    request.on('data', chunk => {
        identifier += chunk.toString('hex'); // convert Buffer to hex string
    });

    request.on('end', () => {

		findUser(identifier, (user) => {

			let current = new Date().getTime()
			let partyID = parseInt(request.params.partyid)
			let remoteIP = clearIP(request.headers['x-forwarded-for'] || request.connection.remoteAddress)

			if (partyID > 1000000) {
				response.send('Invalid id !')
				return;
			}

			console.log(`[${current}][${remoteIP}][${identifier}] New heartbeat in Party ${partyID}`)

			if (user && user != undefined) {
				if (!user.is_ban) {
					let is_exist = (clientList[remoteIP] != undefined)
					clientList[remoteIP] = new Date().getTime();

					if (partyRooms[partyID] == undefined) {
						partyRooms[partyID] = []
						party_nbr++
					}

					if (partyRooms[partyID].indexOf(remoteIP) > -1) {
						response.send('OK')
						return
					}

					if (is_exist) {
					  for (let i = 0, len = partyRooms.length; i < len; i++) {

					  	if (partyRooms[i] != undefined) {
						  	  let index = partyRooms[i].indexOf(remoteIP)
							  if (index > -1) {
							  	if (i != partyID) {

							  		partyRooms[i].splice(index, 1)
						  			partyRooms[partyID].push(remoteIP)
						  			break;
							  	} else if (i == partyID) {
							  		break;
							  	}
							  }
						}

					  }
					} else {
						updateIP(identifier, remoteIP)
						partyRooms[partyID].push(remoteIP)
						client_nbr++
					}

					response.send('OK')
				} else {
					response.send('BAN')
				}
			} else {
				response.send('NOTFOUND')
			}

		})
	})
})

app.post('/username', (request, response) => {
    let identifier = '';

    request.on('data', chunk => {
        identifier += chunk.toString('hex'); // convert Buffer to hex string
    });

    request.on('end', () => {
		findUser(identifier, (user) => {

				let current = new Date().getTime()
				let remoteIP = clearIP(request.headers['x-forwarded-for'] || request.connection.remoteAddress)

				console.log(`[${current}][${remoteIP}][${identifier}] Get username`)

				if (user == undefined || user == null) {
					response.write('')
					response.end()
				} else {
					let username = user.username
					if (user.color != 0) {
						username = '^' + user.color + username
					}

					response.write(username)
					response.end()
				}

		})
	})
})

app.get('/new_party', (request, response) => {
	let current = new Date().getTime()
	let remoteIP = clearIP(request.headers['x-forwarded-for'] || request.connection.remoteAddress)

	let found = 0

	while (!found) {
		let random = Math.floor(Math.random() * 999999) + 1
		if (partyRooms[random] == undefined) {
			found = random
		}
	}

	console.log(`[${current}][${remoteIP}] Available Party ID: ${found}`)
	
	let buffer = Buffer.from( hex32LE( found ) , 'hex')

	response.write( buffer , 'binary')
	response.end(null, 'binary')
})

app.post('/new_player/:username', (request, response) => {
    let identifier = '';

    request.on('data', chunk => {
        identifier += chunk.toString('hex'); // convert Buffer to hex string
    });

    request.on('end', () => {

		let current = new Date().getTime()
		let remoteIP = clearIP(request.headers['x-forwarded-for'] || request.connection.remoteAddress)

		console.log(`[${current}][${remoteIP}][${identifier}] Registering new player ...`)

		if (request.params.username.length < 4 || request.params.username.length > 16 || !validator.isAlphanumeric(request.params.username) ) {
			let buffer = Buffer.from(hex32LE( 1 ), 'hex')
			response.write(buffer, 'binary')
			response.end(null, 'binary')
			return;
		}

		usernameTaken(request.params.username, function(userCount) {

			if (userCount > 0) {
				let buffer = Buffer.from(hex32LE( 2 ), 'hex')
				response.write(buffer, 'binary')
				response.end(null, 'binary')
				return;
			}


		 	ps4Exist (identifier, function(ps4Count) {

			 	if (ps4Count > 0) {
					let buffer = Buffer.from(hex32LE( 3 ), 'hex')
					response.write(buffer, 'binary')
					response.end(null, 'binary')
					return;
				}

				registerUser(identifier, request.params.username, remoteIP, function(registerErr) {
					if (registerErr) {
						let buffer = Buffer.from(hex32LE( 4 ), 'hex')
						response.write(buffer, 'binary')
						response.end(null, 'binary')
						return;
					}

					let buffer = Buffer.from( hex32LE( 5 ) , 'hex')
					response.write(buffer, 'binary')
					response.end(null, 'binary')
					return;
				})

			})
		})
	})
})

app.get('/netip', (request, response) => {
	let current = new Date().getTime()
	let remoteIP = clearIP(request.headers['x-forwarded-for'] || request.connection.remoteAddress)

	console.log(`[${current}][${remoteIP}] External IP given to client`)

	let decimal = ipInt(remoteIP).toInt()
	let buffer = Buffer.from( hex32( decimal ), 'hex')
	response.write(buffer, 'binary')
	response.end(null, 'binary')
})

app.post('/get/:partyid', (request, response) => {
    let identifier = ''

    request.on('data', chunk => {
        identifier += chunk.toString('hex'); // convert Buffer to hex string
    });

    request.on('end', () => {
		findUser(identifier, (user) => {

				let current = new Date().getTime()
				let partyID = parseInt(request.params.partyid)
				let remoteIP = clearIP(request.headers['x-forwarded-for'] || request.connection.remoteAddress)

				if (partyID > 1000000) {
					response.send('Invalid id !')
					return;
				}

				console.log(`[${current}][${remoteIP}] Get clients in Party ${partyID}`)

				let bufferString = ""

				if (user && !user.is_ban) {
					let available = []
					if (partyRooms[partyID] != undefined) {
						partyRooms[partyID].forEach(function(ip) {
							let maxTimeout = clientList[ip] + maxTime
							let currentTime = new Date().getTime()

							// && ip != remoteIP
							if (currentTime < maxTimeout ) {

								ip = clearIP(ip)

								available.push(ip)
							}
						})
					}

					available.forEach(function(ip) {
						let decimal = ipInt(ip).toInt()
						let hexa = hex32LE ( decimal )

						bufferString = bufferString + hexa
					})
				}

				let buffer = Buffer.from(bufferString, 'hex')
				response.write(buffer, 'binary')
				response.end(null, 'binary')

		})
	})
})

// Clean the party list
setInterval(function(){ 
	let party_deleted = 0
	let ip_clean = 0
	let launch_ms = new Date().getTime()

	for (let i = 0, len = partyRooms.length; i < len; i++) {

		if (partyRooms[i] != undefined) {
			partyRooms[i].forEach(function(ip) {
				let maxTimeout = clientList[ip] + maxTime // 2 minutes
				let currentTime = new Date().getTime()

				if (currentTime > maxTimeout) {
					let index = partyRooms[i].indexOf(ip)
					if (index > -1) {
						partyRooms[i].splice(index, 1)
						ip_clean++
					}

					client_nbr--
					delete clientList[ip]
				}
			})

			if (partyRooms[i].length == 0) {
				party_nbr--
				delete partyRooms[i]
				party_deleted++
			}
		}

	}

	let time = new Date().getTime() - launch_ms
	let current = new Date().getTime()

	console.log(`[${current}] ${party_deleted} PartyID deleted and ${ip_clean} IP cleaned in ${time} ms.`)
}, 5 * 60 * 1000);

app.listen(port, (err) => {
	if (err) {
		return console.log('Unable to launch the server', err)
	}

	MongoClient.connect(url, function(err, client) {
		console.log("Connected successfully to server")

	  	if (err) {
			return console.log('Unable to connect to mongodb server', err)
		}

		db = client.db(dbName)
	})

	console.log(`Listening on ${port}`)
})