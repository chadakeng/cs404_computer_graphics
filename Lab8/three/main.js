import * as THREE from 'three';

// orbit camera
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

// Scene setup
const scene = new THREE.Scene();
scene.background = new THREE.Color(0x87ceeb); // Sky blue


// Camera setup
const camera = new THREE.PerspectiveCamera(
    75, // FOV
    window.innerWidth / window.innerHeight, // Aspect ratio
    0.1, // Near plane
    1000 // Far plane
);

camera.position.z = 5; // zoom in/out
// Renderer setup
const renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setPixelRatio(window.devicePixelRatio);
document.body.appendChild(renderer.domElement);

// Controls
// allows us to rotate the camera around the scene
const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;

// Add objects
const geoBox = new THREE.BoxGeometry();
const geoTorus = new THREE.TorusGeometry(1, 0.4, 16, 100);

const matGreen = new THREE.MeshPhongMaterial({
    color: 0x00ff00,
    shininess: 60
});
const matRed = new THREE.MeshPhongMaterial({
    color: 0xff0000,
    shininess: 60
});
const matBlue = new THREE.MeshPhongMaterial({
    color: 0x0000ff,
    shininess: 60
});
const matCopper = new THREE.MeshPhongMaterial({
    color: 0xffe4b5,
    shininess: 60
});

const torus = new THREE.Mesh(geoTorus, matBlue);
const cubeGreen = new THREE.Mesh(geoBox, matGreen);
const cubeRed = new THREE.Mesh(geoBox, matRed);
const sphere = new THREE.Mesh(new THREE.SphereGeometry(0.5, 32, 32), matCopper);

torus.position.x = -2;
torus.rotation.x = 0.5;

sphere.position.x = 0;
sphere.position.y = -2;

cubeGreen.position.x = 2;
cubeGreen.rotation.x = 0.5;

cubeRed.position.y = 2;
cubeRed.rotation.y = 0.5;

scene.add(torus);
scene.add(cubeGreen);
scene.add(cubeRed);
scene.add(sphere);

// Add lighting
const ambientLight = new THREE.AmbientLight(0x404040);
scene.add(ambientLight);
const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
directionalLight.position.set(1, 1, 1);
scene.add(directionalLight);

// Animation update
function animateOld() {
    requestAnimationFrame(animate);
    cubeGreen.rotation.x += 0.001; // note this method will be affected by processing power
    // need time.deltaTime to make the framerate consistent
    cubeRed.rotation.y += 0.09;
    torus.rotation.y += 0.01;
    controls.update();
    renderer.render(scene, camera);
}

// Animation update (with time)
function animate() {
    requestAnimationFrame(animate);
    const time = Date.now() * 0.0005;
    cubeGreen.rotation.x = time * 2;
    cubeRed.rotation.y = time * 20;
    torus.rotation.x += Math.sin(time) * 0.1;

    // scale sphere uniformly with a cosine function with sphere.scale.set()
    sphere.scale.set(
        Math.cos(time) * 0.5 + 1,
        Math.cos(time) * 0.5 + 1,
        Math.cos(time) * 0.5 + 1
    );
    controls.update();
    renderer.render(scene, camera);
}
animate();

// Handle window resize
window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
});




