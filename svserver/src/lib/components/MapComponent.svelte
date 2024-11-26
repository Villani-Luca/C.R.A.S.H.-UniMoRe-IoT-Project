<script lang="ts">
	import { onMount } from 'svelte';

	let map;
	let circle;
	let markers = [];
	let radius = 1000; // Default radius in meters

	type User_Position = {
		latitude: number;
		longitude: number;
	};

	onMount(async () => {
		// Dynamically import Leaflet to avoid SSR issues
		const L = await import('leaflet');
		import('leaflet/dist/leaflet.css');

		let user_position = await getUserPosition();
		debugger;
		console.log(user_position);
		// Initialize the map after Leaflet is imported
		console.log('Posizione ottenuta:', user_position);

		const circle_center = [user_position.latitude, user_position.longitude];

		map = L.map('map', {
			center: [user_position.latitude ?? 100, user_position.longitude ?? 100], // Reggio Emilia, Italy coordinates
			zoom: 13, // Initial zoom level
			minZoom: 2.5, // Restricts zooming out too far
			maxBounds: [
				[-90, -180], // Southwest corner (entire world)
				[90, 180] // Northeast corner (entire world)
			],
			maxBoundsViscosity: 1.0 // Prevents panning beyond the bounds
		});
		L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
			maxZoom: 19,
			attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
		}).addTo(map);

		L.control.scale().addTo(map);
		L.marker([user_position.latitude ?? 100, user_position.longitude ?? 100])
			.addTo(map)
			.bindPopup('Sei Qui!')
			.openPopup();

		circle = L.circle(circle_center, {
			color: 'blue',
			fillColor: '#blue',
			fillOpacity: 0.3,
			radius: radius
		}).addTo(map);
	});

	function addMarker() {
		const marker = L.marker(map.getCenter()).addTo(map);
		markers.push(marker);
		marker.bindPopup('New Marker').openPopup();
	}

	function updateRadius() {
        if (circle) {
            circle.setRadius(radius);
        }
    }

	async function getUserPosition() {
		return new Promise<User_Position>((resolve, reject) => {
			navigator.geolocation.getCurrentPosition((position) => {
				let user_position = {
					latitude: position.coords.latitude,
					longitude: position.coords.longitude
				};
				console.log('Posizione ottenuta:', user_position);
				resolve(user_position);
			}, reject);
		});
	}
</script>


<div class="flex flex-row">
	<div class="flex-grow bg-white border-2 rounded-md p-2 m-2">
		<div id="map"></div>
	</div>
	<div class="bg-white border-2 rounded-md p-4 m-2">
		<div class="settings-panel">
			<div>
				<p class=" font-bold "> Settings Panel</p>
			</div>
			<label for="radius">Adjust Radius (meters):</label>
			<input
				type="range"
				id="radius"
				min="100"
				max="5000"
				bind:value={radius}
				on:input={updateRadius}
			/>
			<label> {radius}</label>
		</div>
	</div>
</div>
<button on:click={() => addMarker()}> add marker</button>


<style>
	#map {
		height: 400px;
		width: 100%;
	}
	button {
		margin-top: 10px;
		padding: 10px;
		cursor: pointer;
	}
</style>
