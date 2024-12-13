const db = require('../model/index.js');
const database = db.database;
const Op = db.sequelize.Op;


let busRequestQueue = new Map();

exports.enroll = async (req, res) => {
    const { Id, type } = req.body;
    const { sequelizeConfig } = db;
    try {
        await sequelizeConfig.transaction(async (t) => {
            const [instance, created] = await database.findOrCreate({
                where: { id: Id },
                defaults: {
                    id: Id,
                    disabledType: type,
                    count: 0
                },
                transaction: t
            });

            if (!created) {
                res.status(200).send({ message: "Already enrolled" });
            } else {
                res.status(200).send({ message: "Enrolled" });
            }
        });
    } catch (err) {
        res.status(500).send({
            message: err.message || 'Error processing transaction.'
        });
    }
};



exports.info = (req, res) => {
    const title = req.query.title;
    let condition = { where: {} };
    let keyword;
    database
        .findAll(condition)
        .then(data => {
            res.send(data);
        })
        .catch(err => {
            res.status(500).send({
                message: err.message || 'Retrieve all tutorials failure.'
            });
        });
};

exports.stop = (req, res) => {
    console.log("ok");
    const { busId, Id, message } = req.body;
    const datas = {
        busId: busId,
        id: Id,
        message: message
    };


    if (busRequestQueue.has(busId)) {

        if (busRequestQueue.get(busId).processing) {
            res.status(429).send({ message: "Bus is in busy" });
            return;
             //Optimistic Lock
        }
        busRequestQueue.get(busId).processing = true; //플래그를 mutex처럼 사용.
        //Pessimistic Lock
        //상태기반 동시성제어
       

        const busData = busRequestQueue.get(busId);
        const busRes = busData.res;
        //console.log(busRes);
        
        busRequestQueue.delete(busId);
        database.findByPk(Id).then(instance => {
            if (instance) {
                busRes.send({ type: instance.disabledType, message: "Data found sent" });
            } else {
                busRes.send({ message: "No data from id" });
                return Promise.resolve(null);  // 후속 처리 중단
            }
        });




        database.findOrCreate({
            where: { id: Id },
            defaults: {
                id: Id,
                count: 0 // 처음 생성할 때 count 값
            }
        }).then(([instance, created]) => {
            if (!created) {
                // 이미 존재하는 경우, count 증가
                return instance.increment('count', { by: 1 });
            }
            return instance;
        }).then(instance => {
            res.status(200).send({ message: "sent to bus", count: instance.count });
        }).catch(err => {
            res.status(500).send({
                message: err.message || 'Error updating or creating entry.'
            });
        });
    } else {
        res.status(200).send({ message: "nothing in queue" });
    }
}

exports.bus = (req, res) => {
    const busId = req.params.busId;

    if (busRequestQueue.has(busId)) {
        busRequestQueue.delete(busId);
    }
    busRequestQueue.set(busId, { res: res, processing: false });
    busRequestQueue.forEach(function (value, key) {
        console.log(`key : ${key} | value : ${value}`);
    });
}

// Create tutorial
exports.create = (req, res) => {
    // Validate request
    if (!req.body.title) {
        res.status(400).send({
            message: 'Title is empty!'
        });
        return;
    }

    // Set tutorial
    const tutorial = {
        title: req.body.title,
        description: req.body.description,
        published: req.body.published ? req.body.published : false
    };

    // Save tutorial
    Tutorial
        .create(tutorial)
        .then(data => {
            res.send(data);
        })
        .catch(err => {
            res.status(500).send({
                message: err.message || 'Create tutorial failure.'
            });
        });
};

// Retrieve all tutorials
exports.findAll = (req, res) => {
    const title = req.query.title;
    let condition = { where: {} };
    let keyword;
    if (keyword) {
        condition = {
            where: {
                [Op.or]: [
                    {
                        title: {
                            [Op.like]: `%${keyword}%`
                        }
                    },
                    {
                        description: {
                            [Op.like]: `%${keyword}%`
                        }
                    }
                ]
            }
        }
    };

    Tutorial
        .findAll(condition)
        .then(data => {
            res.send(data);
        })
        .catch(err => {
            res.status(500).send({
                message: err.message || 'Retrieve all tutorials failure.'
            });
        });
};

// Retrieve tutorial by id
exports.findOne = (req, res) => {
    const id = req.params.id;

    Tutorial
        .findByPk(id)
        .then(data => {
            res.send(data);
        })
        .catch(err => {
            res.status(500).send({
                message: err.message || 'Retrieve tutorial failure. (id: ' + id + ')'
            });
        });
};

// Update tutorial by id
exports.update = (req, res) => {
    const id = req.params.id;
    const condition = id ? { where: { id: id } } : null;

    Tutorial
        .update(
            req.body,
            condition
        )
        .then(resultCount => {
            if (resultCount == 1) {
                res.send({
                    message: 'Tutorial updated.'
                });
            } else {
                res.send({
                    message: 'Cannot update tutorial. (id: ' + id + ')'
                });
            }
        })
        .catch(err => {
            res.status(500).send({
                message: err.message || 'Update tutorial failure. (id: ' + id + ')'
            });
        });
};

// Delete tutorial by id
exports.delete = (req, res) => {
    const id = req.params.id;
    const condition = id ? { where: { id: id } } : null;

    Tutorial
        .destroy(condition)
        .then(resultCount => {
            if (resultCount == 1) {
                res.send({
                    message: 'Tutorial deleted.'
                });
            } else {
                res.send({
                    message: 'Cannot delete tutorial. (id: ' + id + ')'
                });
            }
        })
        .catch(err => {
            res.status(500).send({
                message: err.message || 'Delete tutorial failure. (id: ' + id + ')'
            });
        });
};
