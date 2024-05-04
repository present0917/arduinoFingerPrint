const router = require('express').Router();
const tutorial = require('../controller/controller.js');

// Create tutorial
router.post('/api/tutorial', tutorial.create);

// Retrieve all tutorials
router.get('/api/tutorial', tutorial.findAll);

// Retrieve tutorial by id
router.get('/api/tutorial/:id', tutorial.findOne);

// Update tutorial by id
router.put('/api/tutorial/:id', tutorial.update);

// Delete tutorial by id
router.delete('/api/tutorial/:id', tutorial.delete);

router.post('/api/busstop',tutorial.stop);
router.get('/api/stop/:busId/:userId',tutorial.busstop);
router.get('/api/bus/:busId',tutorial.bus);
//router.post('/api/register',tutorial.register);

module.exports = router;
