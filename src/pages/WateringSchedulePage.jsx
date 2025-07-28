import React, { useEffect, useState } from "react";
import { supabase } from "../lib/supabase";
import { format } from "date-fns";

const frequencyOptions = [
  "Daily",
  "Every 2 days",
  "Every 3 days",
  "Weekly",
];

const WateringSchedulePage = () => {
  const [schedules, setSchedules] = useState([]);
  const [showModal, setShowModal] = useState(false);
  const [formData, setFormData] = useState({
    zone_name: "",
    frequency: "Daily",
    time: "07:00",
    duration: 10,
    status: "Active",
  });

  const fetchSchedules = async () => {
    const { data, error } = await supabase.from("watering_schedules").select("*");
    if (!error) setSchedules(data);
  };

  useEffect(() => {
    fetchSchedules();
  }, []);

  const handleChange = (e) => {
    const { name, value } = e.target;
    setFormData((f) => ({
      ...f,
      [name]: name === "duration" ? parseInt(value) : value,
    }));
  };

  const handleSubmit = async () => {
    const { error } = await supabase.from("watering_schedules").insert([formData]);
    if (!error) {
      setShowModal(false);
      setFormData({ zone_name: "", frequency: "Daily", time: "07:00", duration: 10, status: "Active" });
      fetchSchedules();
    }
  };

  const handleDelete = async (id) => {
    await supabase.from("watering_schedules").delete().eq("id", id);
    fetchSchedules();
  };

  const handleEdit = (s) => {
    setFormData(s);
    setShowModal(true);
  };

  const handleUpdate = async () => {
    const { id, ...updateData } = formData;
    await supabase.from("watering_schedules").update(updateData).eq("id", id);
    setShowModal(false);
    fetchSchedules();
  };

  return (
    <div className="p-6 bg-white dark:bg-gray-900 min-h-screen">
      <div className="flex justify-between items-center mb-6">
        <h1 className="text-2xl font-semibold text-gray-800 dark:text-white">Watering Schedule</h1>
        <button
          onClick={() => setShowModal(true)}
          className="bg-green-600 text-white px-4 py-2 rounded hover:bg-green-700"
        >
          + Add Schedule
        </button>
      </div>

      <div className="overflow-x-auto shadow bg-white dark:bg-gray-800 rounded-lg">
        <table className="min-w-full">
          <thead className="bg-gray-100 dark:bg-gray-700 text-left text-gray-700 dark:text-gray-200">
            <tr>
              <th className="p-3">Zone</th>
              <th className="p-3">Frequency</th>
              <th className="p-3">Time</th>
              <th className="p-3">Duration</th>
              <th className="p-3">Status</th>
              <th className="p-3">Actions</th>
            </tr>
          </thead>
          <tbody>
            {schedules.map((s) => (
              <tr key={s.id} className="border-t border-gray-200 dark:border-gray-700">
                <td className="p-3 text-gray-800 dark:text-gray-100">{s.zone_name}</td>
                <td className="p-3 text-gray-800 dark:text-gray-100">{s.frequency}</td>
                <td className="p-3 text-gray-800 dark:text-gray-100">{format(new Date(`2023-01-01T${s.time}`), "hh:mm a")}</td>
                <td className="p-3 text-gray-800 dark:text-gray-100">{s.duration} mins</td>
                <td className="p-3">
                  <span
                    className={`px-2 py-1 text-sm rounded-full ${
                      s.status === "Active"
                        ? "bg-green-100 text-green-700 dark:bg-green-800 dark:text-green-100"
                        : "bg-gray-200 text-gray-600 dark:bg-gray-600 dark:text-gray-200"
                    }`}
                  >
                    {s.status}
                  </span>
                </td>
                <td className="p-3 space-x-2">
                  <button onClick={() => handleEdit(s)} className="text-green-600 dark:text-green-400 hover:underline">
                    Edit
                  </button>
                  <button onClick={() => handleDelete(s.id)} className="text-red-600 dark:text-red-400 hover:underline">
                    Delete
                  </button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      {/* Smart Tips */}
      <div className="mt-8 p-6 bg-white dark:bg-gray-800 shadow rounded-lg">
        <h2 className="text-lg font-semibold mb-3 text-gray-800 dark:text-white">Smart Watering Tips</h2>
        <ul className="list-disc list-inside text-gray-700 dark:text-gray-200 space-y-1">
          <li>✅ Water early in the morning to reduce evaporation and fungal growth.</li>
          <li>✅ Adjust watering frequency based on weather conditions and season.</li>
          <li>✅ Deep, infrequent watering encourages root growth better than frequent light watering.</li>
          <li>✅ Consider using drip irrigation for efficient water use and healthier plants.</li>
        </ul>
      </div>

      {/* Modal */}
      {showModal && (
        <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-40 z-50">
          <div className="bg-white dark:bg-gray-800 p-6 rounded-lg w-full max-w-md shadow">
            <h3 className="text-xl font-bold mb-4 text-gray-900 dark:text-white">
              {formData.id ? "Edit Schedule" : "Add New Schedule"}
            </h3>
            <div className="space-y-4">
              <label className="block text-sm text-gray-700 dark:text-gray-200">
                Zone Name
                <input
                  name="zone_name"
                  value={formData.zone_name}
                  onChange={handleChange}
                  className="w-full mt-1 border rounded p-2 dark:bg-gray-700 dark:text-white"
                />
              </label>
              <label className="block text-sm text-gray-700 dark:text-gray-200">
                Frequency
                <select
                  name="frequency"
                  value={formData.frequency}
                  onChange={handleChange}
                  className="w-full mt-1 border rounded p-2 dark:bg-gray-700 dark:text-white"
                >
                  {frequencyOptions.map((f) => (
                    <option key={f}>{f}</option>
                  ))}
                </select>
              </label>
              <label className="block text-sm text-gray-700 dark:text-gray-200">
                Time
                <input
                  name="time"
                  type="time"
                  value={formData.time}
                  onChange={handleChange}
                  className="w-full mt-1 border rounded p-2 dark:bg-gray-700 dark:text-white"
                />
              </label>
              <label className="block text-sm text-gray-700 dark:text-gray-200">
                Duration (minutes)
                <input
                  name="duration"
                  type="number"
                  min="1"
                  value={formData.duration}
                  onChange={handleChange}
                  className="w-full mt-1 border rounded p-2 dark:bg-gray-700 dark:text-white"
                />
              </label>
              <label className="block text-sm text-gray-700 dark:text-gray-200">
                Status
                <select
                  name="status"
                  value={formData.status}
                  onChange={handleChange}
                  className="w-full mt-1 border rounded p-2 dark:bg-gray-700 dark:text-white"
                >
                  <option>Active</option>
                  <option>Paused</option>
                </select>
              </label>

              <div className="flex justify-end gap-2 pt-2">
                <button
                  onClick={() => setShowModal(false)}
                  className="px-4 py-2 border rounded dark:border-gray-600 dark:text-white"
                >
                  Cancel
                </button>
                <button
                  onClick={formData.id ? handleUpdate : handleSubmit}
                  className="px-4 py-2 bg-green-600 text-white rounded"
                >
                  {formData.id ? "Update" : "Save"}
                </button>
              </div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default WateringSchedulePage;
