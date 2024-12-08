<script module>
	type Position = {
		latitude: number;
		longitude: number;
	};

	type Device = {
		id: string;
		name: string;
		lat: number;
		long: number;
	};

	type CrashSite = {
		id: number;
		lat: number;
		long: number;
		time: Date;
	};

	export interface Props {
		devices: Device[];
		ondeviceclick?: (device: Device) => Promise<void>;

		crashSites: CrashSite[];

		radius: number;
		activedevice: Device;

		class: string;
	}
</script>

<script lang="ts">
	import type { LatLngExpression } from 'leaflet';
	import type Leaflet from 'leaflet';
	import { onDestroy, onMount } from 'svelte';

	let {
		devices,
		radius,
		crashSites,
		activedevice,
		class: className = '',
		ondeviceclick
	}: Props = $props();

	let map: Leaflet.Map | undefined = $state(undefined);

	let L: typeof import('leaflet') | undefined = undefined;
	let circle: Leaflet.Circle<any> | undefined = undefined;

	let icons_cache: ReturnType<typeof custom_icons> | undefined = undefined;
	let device_markers: { marker: Leaflet.Marker; device: Device; active: boolean }[] = [];
	let crash_markers: { marker: Leaflet.Marker; crash: CrashSite; }[] = [];

	$effect(() => {
		const r = radius * 1000;
		if (circle) {
			circle.setRadius(r);
		}
	});

	$effect(() => {
		// triggers 
		map; activedevice;

		if(L && map){
			icons_cache ??= custom_icons(L);

			for(const device_marker of device_markers){
				if (device_marker.active && device_marker.device.id !== activedevice.id) {
					device_marker.marker.setIcon(icons_cache.device_icon);
					device_marker.active = false;
				}
				else if (!device_marker.active && device_marker.device.id === activedevice.id) {
					device_marker.marker.setIcon(icons_cache.active_device_icon);
					device_marker.active = true;

					circle?.setLatLng(device_marker.marker.getLatLng());
				}
				
				if(device_marker.active){
					map.panTo(device_marker.marker.getLatLng());
				}
			}
		}
	});
	
	$effect(() => {
		// triggers
		map; crashSites; activedevice;

		if(L && map){
			icons_cache ??= custom_icons(L);

			let ids_found: typeof crash_markers = [];
			for(const crashsite of crashSites){
				const found = crash_markers.find(x => x.crash.id === crashsite.id);
				if(!found){
					const marker = L.marker([crashsite.lat, crashsite.long], { icon: icons_cache.crash_icon })
						.addTo(map)
						.bindTooltip(crashsite.time?.toLocaleString());

					const crash_marker = {
						marker: marker,
						crash: crashsite,
					};
					crash_markers.push(crash_marker);
					ids_found.push(crash_marker);
				}
				else {
					ids_found.push(found);
				}
			}

			const removed_crash_markers = crash_markers.filter(x => ids_found.indexOf(x) < 0);
			for(const removed of removed_crash_markers){
				crash_markers.splice(crash_markers.indexOf(removed), 1);
				removed.marker.removeFrom(map);
			}
		}
	})

	onMount(async () => {
		// Dynamically import Leaflet to avoid SSR issues
		try {
			L = await import('leaflet');
			import('leaflet/dist/leaflet.css');

			const map_center: LatLngExpression = [activedevice.lat, activedevice.long];
			map = L.map('map', {
				center: map_center,
				zoom: 13, // Initial zoom level
				minZoom: 2.5, // Restricts zooming out too far
				maxBounds: [
					[-90, -180], // Southwest corner (entire world)
					[90, 180] // Northeast corner (entire world)
				],
				maxBoundsViscosity: 1.0 // Prevents panning beyond the bounds
			});

			icons_cache = custom_icons(L);

			// ##### DEVICES #####
			// Current custom position

			circle = L.circle(map_center, {
				color: 'green',
				fillColor: '#bfffd0',
				fillOpacity: 0.3,
				radius: radius * 1000
			}).addTo(map);

			for (const element of devices ?? []) {
				if (!element.lat || !element.long) continue;

				const is_active_device = element.id === activedevice.id;
				console.log(is_active_device, element);
				const marker = L.marker([element.lat, element.long], {
					icon: is_active_device ? icons_cache.active_device_icon : icons_cache.device_icon
				})
					.bindTooltip(element.name)
					.on('click', async () => await ondeviceclick?.(element))
					.addTo(map);

				device_markers.push({
					device: element,
					marker: marker,
					active: is_active_device
				});
			}

			// ##### CRASHES #####
			for (const element of crashSites ?? []) {
				if (!element.lat || !element.long) {
					return;
				}

				const marker = L.marker([element.lat, element.long], { icon: icons_cache.crash_icon })
					.addTo(map)
					.bindTooltip(element.time.toLocaleString());
					
				crash_markers.push({
					marker: marker,
					crash: element,
				})
			}

			// ##### Map layer #####
			L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
				maxZoom: 19,
				attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
			}).addTo(map);

			L.control.scale().addTo(map);
		} catch (error) {
			console.log(error);
		}
	});

	onDestroy(() => {
		console.debug('Unloading leaflet map');
		map?.remove();
	});

	function custom_icons(L: typeof import('leaflet')) {
		const device_marker_color = '#583470';
		const active_device_marker_color = 'green';
		const crash_marker_color = '#FF0000';

		const marker_html_style = (color: string) => `
  background-color: ${color};
  width: 2rem;
  height: 2rem;
  display: block;
  left: -1rem;
  top: -1rem;
  position: relative;
  border-radius: 3rem 3rem 0;
  transform: rotate(45deg);
  border: 1px solid #FFFFF`;

		const icon_factory = (props: { className: string; style: string }) =>
			L.divIcon({
				className: props.className,
				iconAnchor: [0, 24],
				popupAnchor: [0, -36],
				html: `<span style="${props.style}" />`
			});

		const device_style = marker_html_style(device_marker_color);
		const active_device_style = marker_html_style(active_device_marker_color);
		const crash_style = marker_html_style(crash_marker_color);

		const device_icon = icon_factory({ className: 'device-marker', style: device_style });
		const active_device_icon = icon_factory({
			className: 'active_device-marker',
			style: active_device_style
		});
		const crash_icon = icon_factory({ className: 'crash-marker', style: crash_style });

		return {
			device_icon,
			active_device_icon,
			crash_icon
		};
	}
</script>

<div id="map" class={className}></div>

<style>
	#map {
		height: 400px;
		width: 100%;
	}
</style>
